#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


#define PORT 18000
#define BACKLOG_QUEUE_LEN 10
#define NO_FLAGS 0
#define READ_BUFFER_SIZE 10000
// #define sizeofsockaddr()

int main(int argc, char **argv){

    // allocate a buffer to read requests from client
    char readBuffer[READ_BUFFER_SIZE];
    memset(readBuffer, 0, READ_BUFFER_SIZE);
    
    // check for arguments are valid


    char *response = "HTTP/1.1 200 OK\r\nContent-Length: 16\r\nContent-Type: text/plain\r\n\r\nHello, client!";

    // create listening socket
    int listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

    // bind socket to IP
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind the socket to the specified IP and port
    int bindErr = bind(listeningSocket, (struct sockaddr*) &address, (socklen_t) sizeof(address));

    int listenErr = listen(listeningSocket, BACKLOG_QUEUE_LEN);

    while (1){

        // int newSocket = accept(listeningSocket, (struct sockaddr*) &address, (socklen_t*) sizeof(address));
        int newSocket = accept(listeningSocket, NULL, NULL);

        printf("Accepted a connection..\n");

        read(newSocket, readBuffer, READ_BUFFER_SIZE);

        printf("Client request:\n%s\n", readBuffer);

        // send(newSocket, response, strlen(response), NO_FLAGS);
        write(newSocket, response, strlen(response));

        printf("Sent\n");

        close(newSocket);
    }
    
    return 0;
}