#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <complex.h>

#define STATE_DONE 0
#define STATE_QBASEREADREADY 1
#define STATE_QBITSREADREADY 2
#define STATE_MBASEREADREADY 3
#define STATE_MBITSWRITE 4
#define QPORT 31415
#define MPORT 7071

#define SIG_SENDING_QUREG 128
#define SIG_END_QUREG 129

int connectSocket(int port);
int connectChannel(int listenerSocket);
BitArray readBitArray(int socket);
int setState(int state, int wconnection, int mconnection);
void sendQuReg(int sendSock, quantum_reg *reg);

