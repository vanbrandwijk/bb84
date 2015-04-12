/*
    This file is part of lib-bb84.

    lib-bb84 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    lib-bb84 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with lib-bb84.  If not, see <http://www.gnu.org/licenses/>.

    Copyright J. Joel vanBrandwijk, 2015
*/

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <complex.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include "quantumChannel.h"

int connectListenerSocket(int port) {
	struct sockaddr_in socketAddress;
	int socketID = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &socketAddress, sizeof(socketAddress));
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY;
	socketAddress.sin_port = htons(port);
	bind(socketID, (struct sockaddr *) &socketAddress, sizeof(socketAddress)); 

	if ( socketID < 0 ) {
		printf("ERROR listening on socket\n");
	}

        return socketID;
}

int connectChannel_server(int listenerSocket) {
	int channelSocket, clilen;
	struct sockaddr_in cli_addr;
	
	listen(listenerSocket,0);

	clilen = sizeof(cli_addr);
	channelSocket = accept(listenerSocket, (struct sockaddr *) &cli_addr, &clilen);

	if ( channelSocket < 0 ) {
		printf("ERROR connecting channel socket\n");
		exit(1);
	}

	return channelSocket;
}

int connectChannel_client(char *serverName, int port) {
	int channelSocket;
	struct sockaddr_in serverAddress;
	struct hostent *server;

	channelSocket = socket(AF_INET, SOCK_STREAM, 0);
	server = gethostbyname(serverName);

	bzero((char *) &serverAddress, sizeof(serverAddress));

	serverAddress.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
		(char *)&serverAddress.sin_addr.s_addr, server->h_length);
	serverAddress.sin_port = htons(port);

	connect(channelSocket, 
		(struct sockaddr *) &serverAddress, sizeof(serverAddress));

	return channelSocket;
}

void sendQuReg(int sendSock, quantum_reg *reg) {
	quantum_matrix m;
	char numBuffer[32];
	int i, j;
	float real, imag;

	m = quantum_qureg2matrix((*reg));

	send(sendSock, &(m.cols), sizeof(m.cols), 0);
	send(sendSock, &(m.rows), sizeof(m.rows), 0);

	for ( i = 0; i < m.rows; i++) {
		for ( j = 0; j < m.cols; j++) {
			COMPLEX_FLOAT a = m.t[j + i * m.cols];
			real = creal(a);
			imag = cimag(a);
			sprintf(numBuffer, "%f %f\n", real, imag);
			send(sendSock, numBuffer, sizeof(numBuffer), 0);
		}
	}
}

quantum_reg receiveQuReg(int receiveSock) {
	quantum_matrix m;
	char numBuffer[32];
	int i, j, rows, cols;
	float realPart, imagPart;
	COMPLEX_FLOAT a;

	recv(receiveSock, &cols, sizeof(int), 0);
	recv(receiveSock, &rows, sizeof(int), 0);
	m = quantum_new_matrix(cols, rows);
	for ( i = 0; i < rows; i ++ ) {
		for ( j = 0; j < cols; j++ ) {
			recv(receiveSock, numBuffer, sizeof(numBuffer), MSG_WAITALL);
			sscanf(numBuffer, "%f %f", &realPart, &imagPart);
			m.t[j + i * cols] = realPart + imagPart * I;
		}
	}
	return quantum_matrix2qureg(&m, BIT_ARRAY_LENGTH); 
}

BitArray readBitArray(int receiveSock) {
        BitArray readBitArray;
        char buffer[BIT_ARRAY_LENGTH + 1];
        int i, n;

	initializeBitArray(&readBitArray);
        bzero(&buffer, BIT_ARRAY_LENGTH + 1);

        n = recv(receiveSock, buffer, sizeof(buffer), 0);
        for ( i = 0; i < BIT_ARRAY_LENGTH; i++ ) {
                switch ( buffer[i] ) {
                        case '1':
                                readBitArray.bitArray[i].bit = 1;
                                break;
                        default:
                                readBitArray.bitArray[i].bit = 0;
                                break;
                }
        }
        return readBitArray;
}

void sendBitArray(int sendSock, BitArray b) {
	char buffer[BIT_ARRAY_LENGTH];
	int i;

	for ( i = 0; i < BIT_ARRAY_LENGTH; i++) {
		buffer[i] = b.bitArray[i].bit + '0';
	}
	send(sendSock, buffer, sizeof(buffer), 0);
}

void sendSignal(int sigSocket, int signal) {
	char buffer[BIT_ARRAY_LENGTH];
	bzero(&buffer, sizeof(buffer));

	sprintf(buffer, "SIGNAL %i", signal);
	send(sigSocket, buffer, sizeof(buffer), 0);
} 

void waitSignal(int sigSocket, int signal) {
	int n;
	char buffer[BIT_ARRAY_LENGTH];
	char sigString[BIT_ARRAY_LENGTH];

	sprintf(sigString, "SIGNAL %i", signal);

	while ( 1 ) {
		bzero(&buffer, sizeof(buffer));
	        n = recv(sigSocket, buffer, sizeof(buffer), 0);
		
		if ( strcmp(buffer, sigString) == 0 ) {
			break;
		}
	}
}

float doErrorCheck(int channelSocket, BB84Key k) {
	int i, j, totalError = 0;
	BitArray b, bRemote;
	int numArrays = ( KEY_LENGTH_RAW - KEY_LENGTH ) / BIT_ARRAY_LENGTH;

	for ( i = 0; i < numArrays; i++ ) {
		initializeBitArray(&b);
		for ( j = 0; j < BIT_ARRAY_LENGTH; j++ ) {
			b.bitArray[j].bit = k.key[KEY_LENGTH + BIT_ARRAY_LENGTH * i + j].bit;
		}
		sendBitArray(channelSocket, b);
		bRemote = readBitArray(channelSocket);

		for ( j = 0; j < BIT_ARRAY_LENGTH; j++ ) {
			if ( b.bitArray[j].bit != bRemote.bitArray[j].bit ) {
				totalError++;
			}
		}
	}

	return (float)(totalError / (numArrays * BIT_ARRAY_LENGTH));
}
