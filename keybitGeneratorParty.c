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

/*****
// The party in BB84 encryption scheme which generates the quantum bits
// (Alice)
*****/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "quantumChannel.h"

static const char PARTY_NAME[] = "Alice";

int main() {
	int state = STATE_INITIAL;
	int listenSocket, channelSocket;
	int i, ki = 0;
	
	BitArray encodeBasis, encodeBits, decodeBasis, decodeResponse;
	BB84Key key;
	char finalKey[SHA512_DIGEST_LENGTH];
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
