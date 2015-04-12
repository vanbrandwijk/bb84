#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <quantum.h>
#include <math.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <string.h>
#include "bb84.h"

BitArray generateBitStream(int length, int allZeros) {
	int i, currentBit;
	char data[length];
	BitArray result;

	if ( length > BIT_ARRAY_LENGTH ) {
		for (i = 0; i < BIT_ARRAY_LENGTH; i++) {
			result.bitArray[i].bit = 0;
		}
		return result;
	}

	for (i = 0; i < length; i++) {
		if (allZeros == 1) {
			currentBit = 0;
		} else {
			currentBit = rand() % 2;	//Make a bit
		}
		result.bitArray[i].bit = currentBit;
	}

	return result;
}

void encodeQBitStream(BitArray bits, BitArray basis, quantum_reg *reg) {
	int i;

	for ( i = 0; i < BIT_ARRAY_LENGTH; i++) {
		if ( basis.bitArray[i].bit == 0 ) {
			//Rectalinyear encoding - do nothing
		} else {
			//Orthoganal encoding - rotate by gamma = pi/2 
			//                              (becomes pi/4)
			quantum_r_y(i, 3.1415926/2, reg);
		}
		if ( bits.bitArray[i].bit == 0 ) {
			//Encode a zero - do nothing
		} else {
			//Encode a one - rotate by gamma = pi (becomes pi/2)
			quantum_r_y(i, 3.1415926, reg);
		}
	}
}

BitArray decodeQBitStream(BitArray basis, quantum_reg *reg) {
	int i;
	BitArray bits_decoded;

	for ( i = 0; i < BIT_ARRAY_LENGTH; i++) {
		bits_decoded.bitArray[i].bit = 0;

		if ( basis.bitArray[i].bit == 0 ) {
			// Decode along the rectalinear basis - nothing to do
		} else {
			// Decode along the orthogonal basis - rotate by
			// gamma = -pi/2 (becomes -pi/4)
			quantum_r_y(i, -1 * 3.1415926/2, reg);
		}
		//meassure the bit
		bits_decoded.bitArray[i].bit = quantum_bmeasure_bitpreserve(i, reg);
	}
	return bits_decoded;
}

void initializeBitArray(BitArray *b) {
	int i;
	for ( i = 0; i < BIT_ARRAY_LENGTH; i++ ) {
		(*b).bitArray[i].bit = 0;
	}
}

void printBitArray(BitArray b, char zeros, char ones) {
	int i;
	for ( i = 0; i < BIT_ARRAY_LENGTH; i++ ) {
		if ( b.bitArray[i].bit == 0 ) { 
			printf("%c", zeros); 
		} else {
			printf("%c", ones);
		}
	}
}

void privacyAmp(BB84Key k, char *hash) {
	char buffer[KEY_LENGTH+1];
	int i;

	for ( i = 0; i < KEY_LENGTH; i++ ) {
		sprintf(&buffer[i], "%i", k.key[i].bit);
		
	}

	SHA512(buffer, sizeof(buffer), hash);	
}
