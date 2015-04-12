#include "bb84.h"

int connectSocket(int port);
int connectChannel(int listenerSocket);
BitArray readBitArray(int receiveSock);
int setState(int state, int wconnection, int mconnection);
void sendQuReg(int sendSock, quantum_reg *reg);
quantum_reg receiveQuReg(int receiveSock);
void sendBitArray(int sendSock, BitArray b);
void sendSignal(int sigSocket, int signal);
void waitSignal(int sigSocket, int signal);
float doErrorCheck(int channelSocket, BB84Key k);
