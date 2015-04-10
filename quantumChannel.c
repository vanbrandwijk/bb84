#include <string.h>
#include <netdb.h>
#include "bb84.h"
#include "quantumChannel.h"

int connectListenerSocket(int port) {
	struct sockaddr_in socketAddress;
	int socketID = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &socketAddress, sizeof(socketAddress));
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = INADDR_ANY;
	socketAddress.sin_port = htons(port);
	bind(socketID, (struct sockaddr *) &socketAddress, sizeof(socketAddress)); 

        return socketID;
}

int connectChannel_server(int listenerSocket) {
	int channelSocket, clilen;
	struct sockaddr_in cli_addr;
	
	listen(listenerSocket,0);

	clilen = sizeof(cli_addr);
	channelSocket = accept(listenerSocket, (struct sockaddr *) &cli_addr, &clilen);
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
	char buffer[BIT_ARRAY_LENGTH];
	int i, j;

	sprintf(buffer, "SIG %i", SIG_SENDING_QUREG);
	write(sendSock, buffer, sizeof(buffer));

	m = quantum_qureg2matrix((*reg));

	write(sendSock, &(m.cols), sizeof(m.cols));
	write(sendSock, &(m.rows), sizeof(m.rows));

	for ( i = 0; i < m.rows; i++) {
		for ( j = 0; j < m.cols; j++) {
			COMPLEX_FLOAT a = m.t[j + i * m.cols];
			write(sendSock, creal(a), sizeof(float));
			write(sendSock, cimag(a), sizeof(float));
		}
	}
	sprintf(buffer, "SIG %i", SIG_END_QUREG);
	write(sendSock, buffer, sizeof(buffer));
}

void receiveQuReg(int receiveSock, quantum_reg *reg) {
	quantum_matrix m;
	char buffer[BIT_ARRAY_LENGTH];
	int i, j, rows, cols;
	float realPart, imagPart;
	COMPLEX_FLOAT a;
	
	read(receiveSock, buffer, sizeof(float));
	cols = atoi(buffer);
	
	read(receiveSock, buffer, sizeof(float));
	rows = atoi(buffer);
	
	printf("cols: %f, rows: %f\n", cols, rows);
	m = quantum_new_matrix(cols, rows);
	
	for ( i = 0; i < rows; i ++ ) {
		for ( j = 0; j < cols; j++ ) {
			read(receiveSock, buffer, sizeof(float));
			realPart = atof(buffer);
			read(receiveSock, buffer, sizeof(float));
			imagPart = atof(buffer);
			//m.t[j + i * cols] =
		}
	}
}

BitArray readBitArray(int socket) {
        BitArray readBitArray;
        char buffer[BIT_ARRAY_LENGTH + 1];
        int i, n;

	initializeBitArray(&readBitArray);
        bzero(buffer, BIT_ARRAY_LENGTH + 1);

        n = read(socket, buffer, BIT_ARRAY_LENGTH + 1);

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

