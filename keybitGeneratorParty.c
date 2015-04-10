/*****
// The party in BB84 encryption scheme which generates the quantum bits
// (Alice)
*****/

#include "quantumChannel.h"
#include "bb84.h"

static const char PARTY_NAME[] = "Alice";

int main() {
  int state = STATE_INITIAL;
  int listenSocket, channelSocket;
  int i, ki = 0;
  
  BitArray encodeBasis, encodeBits, decodeBasis, decodeResponse;
  BB84Key key;
  quantum_reg reg;
  
  printf("This is %s.\n", PARTY_NAME);
  listenSocket = connectListenerSocket(BB84_PORT);
  pritnf("Listenting on %i.\n", BB84_PORT);
  
  channelSocket = connectChannel_server(listenSocket);
  printf("Accepted connection... initiating BB84 key exchange.\n");
  state = STATE_KEYEX_INITIATED;
  
  while ( state != STATE_KEYEX_COMPLETE ) {
    switch ( state ) {
      case STATE_KEYEX_INITIATED:
        reg = quantum_new_qureg(0, BIT_ARRAY_LENGTH);
        encodeBasis = generateBitStream(BIT_ARRAY_LENGTH);
        encodeBits = generateBitStream(BIT_ARRAY_LENGTH);
        encodeQBitStream(encodeBits, encodeBasis, &reg);
        sendQuReg(channelSocket, &reg);
        //delete reg
        state = STATE_KEYEX_QBITS_SENT;
        break;
      case STATE_KEYEX_QBITS_SENT:
        decodeBasis = readBitArray(channelSocket);
        for ( i = 0; i < BIT_ARRAY_LENGTH 
          && ak < KEY_LENGTH + BIT_ARRAY_LENGTH * 4; i++ ) {
          if ( encodingBasis.bitArray[i].bit
            == decodeBasis.bitArray[i].bit ) {
            key.key[ki].bit = encodeBits.bitArray[i].bit;
            ki++
            decodeResponse.bitArray[i].bit = 1;
          } else {
            decodeResponse.bitArray[i].bit = 0;
          }
        }
        sendBitArray(channelSocket, decodeResponse);
        state = STATE_KEYEX_INITIATED;
        break;
      }
    }
  }
