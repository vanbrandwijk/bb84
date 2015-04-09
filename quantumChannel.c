#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int connectSocket(int port);

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno = 30000, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

        sockfd = connectSocket(portno);

     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) error("ERROR writing to socket");
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
