#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <quantum.h>
#include <string.h>
#include "bb84.c"

int main () {
  	quantum_reg reg;
 	BitArray bits_to_encode, basis, bits_decoded, decoding_basis, 
		query_decoding_basis, response_decoding_basis;
	BB84Key alice_key, bob_key;
	int i, ak = 0, bk = 0;

	int bitsSent = 0;

	srand(time(0));
	
	while ( ak < KEY_LENGTH + BIT_ARRAY_LENGTH * 4 ) {
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
