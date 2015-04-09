#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <quantum.h>
#include <string.h>

#define BIT_ARRAY_LENGTH (int)8

typedef struct {
	unsigned int bit : 1;
} Bit;

typedef struct {
	Bit bitArray[BIT_ARRAY_LENGTH];
} BitArray;

BitArray generateBitStream(int length, int allZeros);
void encodeQBitStream(BitArray a, BitArray b, quantum_reg *reg);

int main () {
  	quantum_reg reg;
  	int result;
 	BitArray a, b;
	int streamLength = BIT_ARRAY_LENGTH;
	int i;

	srand(time(0));
	
	reg = quantum_new_qureg(0, streamLength);

	a = generateBitStream(streamLength, 0);
	b = generateBitStream(streamLength, 0);

	for ( i = 0; i < streamLength; i++) {
		if ( b.bitArray[i].bit == 0 ) {
			printf("Rect: ", b.bitArray[i].bit);
		} else {
			printf("Orth: ", b.bitArray[i].bit);
			quantum_r_x(i, 3.1415926/2, &reg);
		}
		if ( a.bitArray[i].bit == 0 ) {
		} else {
			quantum_r_y(i, 3.1415926, &reg);
		}
		printf("Bit %i: %i, basis: %i, meassure: (%i)\n", i, a.bitArray[i].bit, b.bitArray[i].bit, quantum_bmeasure(i, &reg) );
	}
	
	return 0;
}

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

void encodeQBitStream(BitArray a, BitArray b, quantum_reg *reg) {

