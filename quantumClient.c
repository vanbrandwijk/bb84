#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <complex.h>
#include "bb84.h"

#define STATE_DONE 0
#define STATE_QBASEREADREADY 1
#define STATE_QBITSREADREADY 2
#define STATE_MBASEREADREADY 3
#define STATE_MBITSWRITE 4
#define QPORT 31415
#define MPORT 7071

#define SIG_SENDING_QUREG 128
#define SIG_END_QUREG 129

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int connectSocket(int port);
int connectChannel(int listenerSocket);
BitArray readBitArray(int socket);
int setState(int state, int wconnection, int mconnection);
void sendQuReg(int sendSock, quantum_reg *reg);

int main(int argc, char *argv[]) {
	quantum_reg reg, newreg;
        BitArray writeBits, writeBases, meassureBases, meassureBits;

        int qmsock, mmsock, qssock, mssock;
	int i, j, state;

	reg = quantum_new_qureg(0, BIT_ARRAY_LENGTH);
	newreg = quantum_new_qureg(0, BIT_ARRAY_LENGTH);

        qmsock = connectSocket(QPORT);
	qssock = connectChannel(qmsock);

        state = STATE_QBASEREADREADY;

        while ( state != STATE_DONE ) {
                switch ( state ) {
                        case STATE_QBASEREADREADY:
                                writeBases = readBitArray(qssock);
                                state = STATE_QBITSREADREADY;
                                break;
                        case STATE_QBITSREADREADY:
                                writeBits = readBitArray(qssock);
				encodeQBitStream(writeBits, writeBases, &reg);
				sendQuReg(qssock, &reg);
				state = STATE_QBASEREADREADY;
                                break;
                        default:
                                break;
                }
        }
                for ( i = 0; i < BIT_ARRAY_LENGTH; i++ ) {
                        printf("%i", writeBases.bitArray[i].bit);
                        printf("%i", writeBits.bitArray[i].bit);
                        printf("%i", meassureBases.bitArray[i].bit);
                printf("\n");
                }
quantum_print_qureg(reg);

	close(mssock);
	close(qssock);
	close(mmsock);
	close(qmsock);

	return 0;
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
	

int connectSocket(int port) {
     struct sockaddr_in socketAddress;
     int socketID = socket(AF_INET, SOCK_STREAM, 0);
     if (socketID < 0)
        error("ERROR opening socket");
     bzero((char *) &socketAddress, sizeof(socketAddress));
     socketAddress.sin_family = AF_INET;
     socketAddress.sin_addr.s_addr = INADDR_ANY;
     socketAddress.sin_port = htons(port);
     if (bind(socketID, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) < 0)
              error("ERROR on binding");

        return socketID;
}

BitArray readBitArray(int socket) {
        BitArray readBitArray;
        char buffer[BIT_ARRAY_LENGTH + 1];
        int i, n;

	initializeBitArray(&readBitArray);
        bzero(buffer, BIT_ARRAY_LENGTH + 1);

        n = read(socket, buffer, BIT_ARRAY_LENGTH + 1);
        if ( n < 0 ) error("ERROR reading from socket");

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

int setState(int state, int wconnection, int mconnection) {
	return state;
}

int connectChannel(int listenerSocket) {
	int channelSocket, clilen;
	struct sockaddr_in cli_addr;
	
	listen(listenerSocket,0);

	clilen = sizeof(cli_addr);
	channelSocket = accept(listenerSocket, (struct sockaddr *) &cli_addr, &clilen);
	if (channelSocket < 0)
		error("ERROR on accept");
	return channelSocket;
}
