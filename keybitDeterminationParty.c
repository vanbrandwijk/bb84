/*****
// The party in BB84 encryption scheme which meassures the quantum bits
// (Bob)
*****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "quantumChannel.h"

static const char PARTY_NAME[] = "Bob";

int main(int argc, char *argv[]) {
	int state = STATE_INITIAL;
	int channelSocket;
	int i, ki = 0;
	
	BitArray decodeBasis, decodedBits, decodeResponse;
	BB84Key key;
	char finalKey[SHA512_DIGEST_LENGTH];
	quantum_reg reg;

	srand(time(0));
	
	printf("This is %s.\n", PARTY_NAME);
 
	channelSocket = connectChannel_client(argv[1], BB84_PORT);
	printf("Connected to %s... initiating BB84 key exchange.\n", argv[0]);
	
	state = STATE_KEYEX_INITIATED;
	
	while ( state != STATE_KEYEX_COMPLETE && ki < KEY_LENGTH_RAW ) {
		switch ( state ) {
			case STATE_KEYEX_INITIATED:
				initializeBitArray(&decodeBasis);
				decodeBasis = generateBitStream(BIT_ARRAY_LENGTH, 0);
				printf("Decoding basis:\t");
				printBitArray(decodeBasis, '+', 'x');
				printf("\n");

				sendSignal(channelSocket, SIG_DETREADY);
				waitSignal(channelSocket, SIG_SENDINGQBITS);
				reg = receiveQuReg(channelSocket);
				initializeBitArray(&decodedBits);
				decodedBits = decodeQBitStream(decodeBasis, &reg);
				printf("Decoded bits:\t");
				printBitArray(decodedBits, '0', '1');
				printf("\n");

				sendSignal(channelSocket, SIG_SENDINGMBASIS);
				sendBitArray(channelSocket, decodeBasis);
				//delete reg
				state = STATE_KEYEX_QBITS_SENT;
				break;
			case STATE_KEYEX_QBITS_SENT:
				waitSignal(channelSocket, SIG_SENDINGAGREE);
				decodeResponse = readBitArray(channelSocket);
				printf("Correct bits:\t");
				printBitArray(decodeResponse, ' ', '^');
				printf("\n");

				for ( i = 0; i < BIT_ARRAY_LENGTH 
					&& ki < KEY_LENGTH_RAW; i++ ) {
					if ( decodeResponse.bitArray[i].bit == 1 ) {
						key.key[ki].bit = decodedBits.bitArray[i].bit;
						ki++;
					}
				}
				state = STATE_KEYEX_INITIATED;
				break;
		}
	}

	//perform error checking
	printf("Error occurance: %f\n", doErrorCheck(channelSocket, key));	

	//perform privacy amplification
	privacyAmp(key, &finalKey);

	printf("%s's privacy amplified key: \n", PARTY_NAME);

	for ( i = 0; i < sizeof(finalKey); i++ ) {
		printf("%02x", finalKey[i] & 0xff);
	}
	printf("\n");
}
