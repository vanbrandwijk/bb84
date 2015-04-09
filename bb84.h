#include <stdlib.h>
#include <quantum.h>

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

typedef struct {
	unsigned int type;
} BB84NetworkMessage;

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

//Send & recieve quantum data
int sendQuantum(quantum_reg reg);
int recieveQuantum(quantum_reg *reg);

//Send & recieve classical data
int sendClassic(BitArray s_bits);
int recieveClassic(BitArray r_bits);
