#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <quantum.h>
#include <string.h>

//The bit array length defines the width of the quantum register needed on
//either side of the transaction.  This width should be scaled to the available
//quantum register implementation, but must be in agreement on each side.
#define BIT_ARRAY_LENGTH (int)16

//The key length.  This will be the length of the final key after the BB84 key
//exchange is complete
#define KEY_LENGTH (int)1024

//Define a struct to contain a bit
typedef struct {
	unsigned int bit : 1;
} Bit;

//Define a struct to contain an array with length equal to the width of the
//quantum register
typedef struct {
	Bit bitArray[BIT_ARRAY_LENGTH];
} BitArray;

//Define a struct to contain an array of bits.  This array will contain the key
//as well as the "spare bits" to be used in detection of eavesdropping
typedef struct {
	Bit key[KEY_LENGTH+BIT_ARRAY_LENGTH*4];
} BB84Key;

//Generate a stream of bits - either random or all zeros if the second
//parameter is > 0
BitArray generateBitStream(int length, int allZeros);

//Encode a stream of bits to qbits into rectangular and diagonal polarizations,
//given an array of bits, an array of bases, and the quantum register in which
//to store the results
void encodeQBitStream(BitArray bits, BitArray basis, quantum_reg *reg);

//Decode a stream of qbits into bits, given an array of bases along which to 
//decode, and the quantum register.  Return the decoded bits
BitArray decodeQBitStream(BitArray basis, quantum_reg *reg);

int main () {
  	quantum_reg reg;
 	BitArray bits_to_encode, basis, bits_decoded, decoding_basis, 
		query_decoding_basis, response_decoding_basis;
	BB84Key alice_key, bob_key;
	int i, q, ak = 0, bk = 0;

	srand(time(0));
	
	for (q = 0; ak < KEY_LENGTH + BIT_ARRAY_LENGTH * 4; q++) {
		for ( i = 0; i < BIT_ARRAY_LENGTH; i++) {
			bits_to_encode.bitArray[i].bit = 0;
			basis.bitArray[i].bit = 0;
			bits_decoded.bitArray[i].bit = 0;
			decoding_basis.bitArray[i].bit = 0;
			query_decoding_basis.bitArray[i].bit = 0;
			response_decoding_basis.bitArray[i].bit = 0;
		}

		reg = quantum_new_qureg(0, BIT_ARRAY_LENGTH);
	
		/*****
		//Alice picks random bases and bits, and encodes them in a quantum reg
		*****/
		bits_to_encode = generateBitStream(BIT_ARRAY_LENGTH, 0);
		basis = generateBitStream(BIT_ARRAY_LENGTH, 0);

		encodeQBitStream(bits_to_encode, basis, &reg);

		/*****
		// Bob picks random bases and decodes bits from the quantum reg
		*****/
		decoding_basis = generateBitStream(BIT_ARRAY_LENGTH, 0);
		bits_decoded = decodeQBitStream(decoding_basis, &reg);

		/*****
		// Bob sends his bases to Alice
		*****/	
		//query_decoding_basis = decoding_basis;
	
		/*****
		// Alice recieves Bob's decoding basis and compares it to the encoding
		// basis.  She adds the correctly decoded bits to her key and responds
		// with the indexes of the bits which were decoded correctly
		*****/
		for ( i = 0; i < BIT_ARRAY_LENGTH 
			&& ak < KEY_LENGTH + BIT_ARRAY_LENGTH * 4; i++ ) {
			if ( basis.bitArray[i].bit 
				== decoding_basis.bitArray[i].bit ) {
				alice_key.key[ak].bit = bits_to_encode.bitArray[i].bit;
				printf("A: Basis array %i, bit value %i, key position %i\n", i, bits_to_encode.bitArray[i].bit, ak); 
				ak++;
				response_decoding_basis.bitArray[i].bit = 1;
			}
		}
	
		/*****
		// Bob recieves the response, and places the bits which were correctly
		// decoded into his key
		*****/
		for ( i = 0; i < BIT_ARRAY_LENGTH; i++ ) {
			if ( response_decoding_basis.bitArray[i].bit == 1 ) {
				bob_key.key[bk].bit = bits_decoded.bitArray[i].bit;
				printf("B: Basis array %i, bit value %i, key position %i\n", i, bits_decoded.bitArray[i].bit, bk); 
				bk++;
			}
		}
	
	}
	
	/*****
	// Let's check out those keys!
	*****/
	printf("Alice's key: vvv \n");
	for ( i = 0; i < KEY_LENGTH; i++ ) {
		printf("%i", alice_key.key[i].bit);
	}
	printf(" | spare: ");
	for ( i = KEY_LENGTH; i < KEY_LENGTH + BIT_ARRAY_LENGTH * 4; i++ ) {
		printf("%i", alice_key.key[i].bit);
	}
	printf("\n\n");
	for ( i = 0; i < KEY_LENGTH; i++ ) {
		printf("%i", bob_key.key[i].bit);
	}
	printf(" | spare: ");
	for ( i = KEY_LENGTH; i < KEY_LENGTH + BIT_ARRAY_LENGTH * 4; i++ ) {
		printf("%i", bob_key.key[i].bit);
	}
	printf("\nBob's key: ^^^\n");

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
