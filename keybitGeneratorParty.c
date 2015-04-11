/*****
// The party in BB84 encryption scheme which generates the quantum bits
// (Alice)
*****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quantumChannel.h"

static const char PARTY_NAME[] = "Alice";

int main() {
	int state = STATE_INITIAL;
	int listenSocket, channelSocket;
	int i, ki = 0;
	
	BitArray encodeBasis, encodeBits, decodeBasis, decodeResponse;
	BB84Key key;
	quantum_reg reg;

	srand(time(0));

	printf("This is %s.\n", PARTY_NAME);
	listenSocket = connectListenerSocket(BB84_PORT);
	printf("Listenting on %i.\n", BB84_PORT);
	
	channelSocket = connectChannel_server(listenSocket);
	printf("Accepted connection... initiating BB84 key exchange.\n");
	state = STATE_KEYEX_INITIATED;

	
	while ( state != STATE_KEYEX_COMPLETE && ki < KEY_LENGTH_RAW ) {
		switch ( state ) {
			case STATE_KEYEX_INITIATED:
				waitSignal(channelSocket, SIG_DETREADY);
				reg = quantum_new_qureg(0, BIT_ARRAY_LENGTH);
				initializeBitArray(&encodeBits);
				encodeBits = generateBitStream(BIT_ARRAY_LENGTH, 0);
				printf("Bits: \t");
				printBitArray(encodeBits, '0', '1');
				printf("\n");				

				initializeBitArray(&encodeBasis);
				encodeBasis = generateBitStream(BIT_ARRAY_LENGTH, 0); 
				printf("Bases: \t");
				printBitArray(encodeBasis, '+', 'x');
				printf("\n");
				
				encodeQBitStream(encodeBits, encodeBasis, &reg);
				sendSignal(channelSocket, SIG_SENDINGQBITS);
				sendQuReg(channelSocket, &reg);
				//delete reg
				state = STATE_KEYEX_QBITS_SENT;
				break;
			case STATE_KEYEX_QBITS_SENT:
				waitSignal(channelSocket, SIG_SENDINGMBASIS);
				decodeBasis = readBitArray(channelSocket);

				printf("Agree:\t");
				for ( i = 0; i < BIT_ARRAY_LENGTH 
					&& ki < KEY_LENGTH_RAW; i++ ) {
					if ( encodeBasis.bitArray[i].bit
						== decodeBasis.bitArray[i].bit ) {
						key.key[ki].bit = encodeBits.bitArray[i].bit;
						ki++;
						decodeResponse.bitArray[i].bit = 1;
						printf("^");
					} else {
						decodeResponse.bitArray[i].bit = 0;
						printf(" ");
					}
				}
				printf("\n");

				sendSignal(channelSocket, SIG_SENDINGAGREE);
				sendBitArray(channelSocket, decodeResponse);
				state = STATE_KEYEX_INITIATED;
				break;
		}
	}

	printf("%s's key:\n", PARTY_NAME);
	for ( i = 0; i < ki; i++ ) {
		printf("%i", key.key[i].bit);
	}
	printf("\n");
}
