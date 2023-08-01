#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //for sockaddr_in
#include <string.h>
#include <unistd.h> //for close
#include <errno.h> //for errors


#define PORT 18000
#define BACKLOG_QUEUE_LEN 10
#define NO_FLAGS 0
#define READ_BUFFER_SIZE 10000
// #define sizeofsockaddr()
#define handleError(errMsg) {\
        fprintf(stderr, "%s\n Error cause:  %d: %s\n", errMsg, errno, strerror(errno));\
        exit(1);\
    }


int main(int argc, char **argv){

    // allocate a buffer to read requests from client
    char readBuffer[READ_BUFFER_SIZE];
    memset(readBuffer, 0, READ_BUFFER_SIZE);
    
    // check for arguments are valid
    // for now no command-line-arguments

    char *response = "HTTP/1.1 200 OK\r\nContent-Length: 16\r\nContent-Type: text/plain\r\n\r\nHello, client!";

    // create listening socket
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket < 0)
    {
        handleError("Error when initializing socket");
    }
    

    // bind socket to IP
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind the socket to the specified IP and port
    if(bind(listeningSocket, (struct sockaddr*) &address, (socklen_t) sizeof(address)) < 0){
        handleError("Error when binding address to socket");
    }

    if(listen(listeningSocket, BACKLOG_QUEUE_LEN) < 0){
        handleError("Error when setting listening mode to socket");
    }

    while (1){

        // int newSocket = accept(listeningSocket, (struct sockaddr*) &address, (socklen_t*) sizeof(address));
        int newSocket = accept(listeningSocket, NULL, NULL);
        if (newSocket < 0){
            handleError("Error when accepting connection request");
        }
        

        printf("Accepted a connection..\n");

        if(read(newSocket, readBuffer, READ_BUFFER_SIZE) < 0){
            handleError("Error when reading request");
        }

        // printf("Client request:\n%s\n", readBuffer);

        // send(newSocket, response, strlen(response), NO_FLAGS);
        if(write(newSocket, response, strlen(response)) < 0){
            handleError("Error when writing to socket");
        }

        printf("Sent\n");

        close(newSocket);
    }
    
    return 0;
}