/*****
// The party in BB84 encryption scheme which meassures the quantum bits
// (Bob)
*****/

#include "quantumChannel.h"
#include "bb84.h"

static const char PARTY_NAME[] = "Bob";

int main(int argc, char *argv[]) {
  int state = STATE_INITIAL;
  int channelSocket;
  int i, ki = 0;
  
  BitArray  decodeBasis, decodedBits, decodeResponse;
  BB84Key key;
  quantum_reg reg;
  
  printf("This is %s.\n", PARTY_NAME);
 
  channelSocket = connectChannel_client(argv[0], BB84_PORT);
  printf("Connected to %s... initiating BB84 key exchange.\n", argv[0]);
  
  state = STATE_KEYEX_INITIATED;
  
  while ( state != STATE_KEYEX_COMPLETE ) {
    switch ( state ) {
      case STATE_KEYEX_INITIATED:
        reg = quantum_new_qureg(0, BIT_ARRAY_LENGTH);
        decodeBasis = generateBitStream(BIT_ARRAY_LENGTH);
        recieveQuReg(channelSocket, &reg);
        decodedBits = decodeQBitStream(decodeBasis, &reg);
        sendBitArray(channelSocket, decodeBasis);
        //delete reg
        state = STATE_KEYEX_QBITS_SENT;
        break;
      case STATE_KEYEX_QBITS_SENT:
        decodeResponse = readBitArray(channelSocket);
        for ( i = 0; i < BIT_ARRAY_LENGTH; i++ ) {
          if ( decodeResponse.bitArray[i].bit == 1 ) {
            key.key[ki].bit = decodedBits.bitArray[i].bit;
            ki++;
          }
        }
        state = STATE_KEYEX_INITIATED;
        break;
    }
  }
}
