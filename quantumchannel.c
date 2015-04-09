#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define QPORT (int)31415
#define CPORT (int)7071

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
	int qSock_r, qSock_s, cSock_r, cSock_s;
	socklen_t qSockClientLen, cSockClientLen;
	char qBuffer[32], cBuffer[16];

	struct sockaddr_in qServerAddr, qClientAddr, cServerAddr, cClientAddr;

	int n;

	qSock_r = socket(AF_INET, SOCK_STREAM, 0);
	if ( qSock_r < 0 ) 
		error("Could not open server direction of quantum socket\n");
	cSock_r = socket(AF_INET, SOCK_STREAM, 0);
	if ( qSock_r < 0 ) 
		error("Could not open server direction of classic socket\n");

	bzero((char *) &qServerAddr, sizeof(qServerAddr));
	bzero((char *) &cServerAddr, sizeof(cServerAddr));

	qServerAddr.sin_family = AF_INET;
	qServerAddr.sin_addr.s_addr = INADDR_ANY;
	qServerAddr.sin_port = htons(QPORT);
	if ( bind(qSock_r, (struct sockaddr *) &qServerAddr, sizeof(qServerAddr)) < 0 ) 
		error("Could not bind server direction of quantum socket\n");

	cServerAddr.sin_family = AF_INET;
	cServerAddr.sin_addr.s_addr = INADDR_ANY;
	cServerAddr.sin_port = htons(CPORT);
	if ( bind(cSock_r, (struct sockaddr *) &cServerAddr, sizeof(cServerAddr)) < 0 ) 
		error("Could not open server direction of classic socket\n");

	listen(qSock_r, 1);
	qSockClientLen = sizeof(qClientAddr);
	qSock_s = accept(qSock_r, (struct sockaddr *) &qClientAddr, &qSockClientLen);
	
	if ( qSock_s < 0 )
		error("Coult not accpet connection on quantum socket\n");

	bzero(qBuffer, 32);

	n = read(qSock_s, qBuffer, 32);

	if ( n < 0 )
		error("Could not read on quantum socket");

	printf("Here is the message: %s\n",qBuffer);

	close(qSock_s);
	close(qSock_r);

	return 0; 
}
