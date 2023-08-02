#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //for sockaddr_in
#include <string.h>
#include <unistd.h> //for close
#include <errno.h> //for errors
#include <signal.h>


#define PORT 18000
#define BACKLOG_QUEUE_LEN 10
#define NO_FLAGS 0
#define READ_BUFFER_SIZE 10000
#define RESOURCE_URL_SIZE 50
#define MAX_RESPONSE_SIZE 1000
#define handleError(errMsg) {\
        fprintf(stderr, "%s\nError cause:  %d: %s\n", errMsg, errno, strerror(errno));\
        exit(1);\
    }

void send404Error(int clientSocket){

    const char *not_found_response = 
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 15\r\n\r\n"
        "404 Not Found\n";
    
    if (send(clientSocket, not_found_response, strlen(not_found_response), 0) < 0) {
            close(clientSocket);
            handleError("Error sending file (Inside sendFileRespons)");
    }

    close(clientSocket);
    
}

void sendFileResponse(int client_socket, const char *filename) {

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        handleError("Error opening file");
    }

    char response[MAX_RESPONSE_SIZE];
    int bytes_read;

    while ((bytes_read = fread(response, 1, sizeof(response), file)) > 0) {
        if (send(client_socket, response, bytes_read, 0) != bytes_read) {
            fclose(file);
            close(client_socket);
            handleError("Error sending file (Inside sendFileRespons)");
        }
    }

    close(client_socket);

    return;

}

void handleRequest(int clientSocket, char* readBuffer){

    // extract part after the root ("/") symbol
    // check if it is a txt file
        // if txt then give to a handleTxtFile fn
    // if ico then handle using handleIcoFile (only if its favicon)
    // else return 404 error code

    char delimeter = '\r';
    char *pos = strchr(readBuffer, delimeter);

    char resourceRequested[RESOURCE_URL_SIZE];

    strncpy(resourceRequested, readBuffer, pos - readBuffer);
    resourceRequested[pos - readBuffer] = '\0';


    if(strcmp(resourceRequested, "GET / HTTP/1.1") == 0){
        sendFileResponse(clientSocket, "hello.txt");
    }
    else if(strcmp(resourceRequested, "GET /favicon.ico HTTP/1.1") == 0){
        sendFileResponse(clientSocket, "ucsc-logo.png");

    }
    else if(strcmp(resourceRequested, "GET /a.txt HTTP/1.1") == 0){
        printf("3\n");
        sendFileResponse(clientSocket, "a.txt");
    }
    else if(strcmp(resourceRequested, "GET /b.txt HTTP/1.1") == 0){
        printf("4\n");
        sendFileResponse(clientSocket, "b.txt");
    }
    else{
        send404Error(clientSocket);
    }

    return;

}

int main(int argc, char** argv){

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

    // signal(SIGINT, intHandler, listeningSocket)
    
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

        // printf("Accepted a connection..\n");

        if(recv(newSocket, readBuffer, READ_BUFFER_SIZE, 0) < 0){
            handleError("Error when reading request");
        }

        // printf("Client request:\n%s\n", readBuffer);

        handleRequest(newSocket, readBuffer);

        // if (strncmp(readBuffer, "GET /favicon.ico", 16) == 0)
        // {
        //     // should send a image here for icon of the tab
        // }
        

        // // send(newSocket, response, strlen(response), NO_FLAGS);
        // if(write(newSocket, response, strlen(response)) < 0){
        //     handleError("Error when writing to socket");
        // }

        // printf("Sent\n");

        close(newSocket);
    }
    
    return 0;
}