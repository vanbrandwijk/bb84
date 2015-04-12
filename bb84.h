#include <stdlib.h>
#include <quantum.h>

#define STATE_INITIAL 0
#define STATE_KEYEX_INITIATED 1
#define STATE_KEYEX_QBITS_SENT 2
#define STATE_KEYEX_EC 3
#define STATE_KEYEX_COMPLETE 128

#define SIG_RESET 0
#define SIG_DETREADY 1
#define SIG_SENDINGQBITS 2
#define SIG_SENDINGMBASIS 3
#define SIG_SENDINGAGREE 4

#define BB84_PORT (int)31415

#define BB84_ERROR_TOLERANCE (float).02

//The bit array length defines the width of the quantum register needed on
//either side of the transaction.  This width should be scaled to the available
//quantum register implementation, but must be in agreement on each side.
#define BIT_ARRAY_LENGTH (int)16

//The key length.  This will be the length of the final key after the BB84 key
//exchange is complete
#define KEY_LENGTH (int)1024
#define KEY_LENGTH_RAW KEY_LENGTH + BIT_ARRAY_LENGTH * 4

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
	Bit key[KEY_LENGTH_RAW];
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

void initializeBitArray(BitArray *b);
void printBitArray(BitArray b, char zeros, char ones);
