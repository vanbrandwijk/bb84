#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "bb84.h"

#define STATE_DONE 0
#define STATE_QREADREADY 1

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int connectSocket(int port);
BitArray readBitArray(int socket);

int main(int argc, char *argv[])
{
        BitArray writeBits, writeBases, meassureBases, meassureBits;

     int sockfd, newsockfd, portno = 31415, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int i, n, state;

        sockfd = connectSocket(portno);

     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");

        state = STATE_QREADREADY;

        while ( state != STATE_DONE ) {
                writeBits = readBitArray(newsockfd);

                for ( i = 0; i < BIT_ARRAY_LENGTH; i++ ) {
                        printf("%i", writeBits.bitArray[i].bit);
                }
                printf("\n");
        }

     return 0;
}

int connectSocket(int port) {
     struct sockaddr_in socketAddress;
     int socketID = socket(AF_INET, SOCK_STREAM, 0);
     if (socketID < 0)
        error("ERROR opening socket");
     bzero((char *) &socketAddress, sizeof(socketAddress));
     socketAddress.sin_family = AF_INET;
     socketAddress.sin_addr.s_addr = INADDR_ANY;
     socketAddress.sin_port = htons(port);
     if (bind(socketID, (struct sockaddr *) &socketAddress, sizeof(socketAddress)) < 0)
              error("ERROR on binding");

        return socketID;
}

BitArray readBitArray(int socket) {
        BitArray readBitArray;
        char buffer[BIT_ARRAY_LENGTH + 1];
        int i, n;

        bzero(buffer, BIT_ARRAY_LENGTH + 1);

        n = read(socket, buffer, BIT_ARRAY_LENGTH + 1);
        if ( n < 0 ) error("ERROR reading from socket");

        for ( i = 0; i < BIT_ARRAY_LENGTH; i++ ) {
                switch ( buffer[i] ) {
                        case '1':
                                readBitArray.bitArray[i].bit = 1;
                                break;
                        default:
                                readBitArray.bitArray[i].bit = 0;
                                break;
                }
        }
        return readBitArray;
}
