#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <quantum.h>
#include <string.h>
#include "bb84.c"

#define BB84_SERVER_NOCON (int)0;
#define BB84_SERVER_SENT_QKEY (int)1;
#define BB84_SERVER_SENT_ACKB (int)2;
#define BB84_SERVER_KEYCOMPLETE (int)4;
#define BB84_SERVER_ERRCHK (int)8;
#define BB84_SERVER_RESET (int)16;
#define BB84_SERVER_CON (int)32;

#define BB84_MSG_QREQ (int)0;
#define BB84_MSG_QKEY (int)1;
#define BB84_MSG_CLIENT_BASES (int)2;
#define BB84_MSG_KEYCOMPLETE (int)4;
#define BB84_MSG_ERRBITS (int)8;
#define BB84_MSG_REJECT (int)16;
#define BB84_MSG_ACCEPT (int)32;

BB84NetworkMessage recieve_network_message(); 
int send_network_message(BB84NetworkMessage message);
int sendQuantumReg();
int sendCorrectBases(BitArray basis, BB84NetworkMessage c_message); 

int main () {
  	quantum_reg reg;
 	BitArray bits, basis;  
	BB84Key key;
	int i, ak = 0;
	int state = Q_BB84_NOCON;
	BB84NetworkMessage s_message, c_message;

	srand(time(0));
	
	while ( state != Q_BB84_CON ) {
		c_message = recieve_network_message();

		switch(state) {
			case BB84_NOCON:
			case BB84_SENT_ACKB:
				if ( c_message.type == BB84_MSG_QREQ ) {
					state = sendQuantumReg();
					break;
				}
			case BB84_SENT_QKEY:
				if ( c_message.type == BB84_MSG_CLIENT_BASES ) {
					state = sendCorrectBases(basis, c_message);
					break;
				}
			case BB84_SERVER_KEYCOMPLETE:
				if ( c_message.type == BB84_MSG_KEYCOMPLETE ) {
					state = sendErrorCheck(key);
					break;
				}
			default:
				sendConnectionReset();
				break;
		}
	}
	return 0;
}

int sendQuantumReg() {
	return BB84_SENT_QKEY;
}	

int sendCorrectBases(BitArray basis, BB84NetworkMessage c_message) {
	return BB84_SENT_ACKB;
}
