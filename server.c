#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //for sockaddr_in
#include <string.h>
#include <unistd.h> //for close
#include <errno.h> //for errors
#include <signal.h>
#include <pthread.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "config.h"
#include "utils.h"
#include "error.h"

void sendFileResponse(int client_socket, char *filename) {

    char filepath[100];

    // Concatenate "content/" and the filename
    strcpy(filepath, "content/");
    strcat(filepath, filename);

    int file = open(filepath, O_RDONLY);
    if (file < 0) {
        send404Error(client_socket);
        return;
    }

    // find the contentType
    char contentType[CONTENT_TYPE_SIZE];

    getContentType(filename, contentType);
        
    struct stat file_stat;
    fstat(file, &file_stat);

    // Send HTTP headers
    const char *response_headers = "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: %s\r\n"
                                    "Content-Length: %ld\r\n"
                                    "\r\n";
    char headers_buffer[1024]; // Adjust buffer size as needed
    sprintf(headers_buffer, response_headers, contentType, file_stat.st_size);

    send(client_socket, headers_buffer, strlen(headers_buffer), 0);

    if(sendfile(client_socket, file, NULL, file_stat.st_size) < 0){
        close(file);
        close(client_socket);
        handleError("Error in sendFile()");
    }

    close(client_socket);

    return;

}

void* handleRequest(void* clientSocketPtr){

    int clientSocket = *((int*)clientSocketPtr);

    // allocate a buffer to read requests from client
    char readBuffer[READ_BUFFER_SIZE];
    memset(readBuffer, 0, READ_BUFFER_SIZE);

    if(recv(clientSocket, readBuffer, READ_BUFFER_SIZE, 0) < 0){
        handleError("Error when reading request");
    }

    const char delim[] = " ";

    char *token = strtok(readBuffer, delim);
    // check if first three characters are GET if not return
    if(strcmp(token, "GET") != 0){
        send404Error(clientSocket);
    }

    // get requested file name
    token = strtok(NULL, delim);

    if (strlen(token) == 1)
    {
        sendFileResponse(clientSocket, "hello.txt");
        close(clientSocket);

        return NULL;
    }
    

    sendFileResponse(clientSocket, token+1);


    sleep(20);
    close(clientSocket);
    free(clientSocketPtr);

    return NULL;
}

int main(int argc, char** argv){
    
    // check for arguments are valid
    // for now no command-line-arguments
    if (argc != 2){
        handleError("Please specify port to run as an argument");
    }
    
    int PORT = atoi(argv[1]);

    if (PORT < 1024 && PORT > 49151)
    {
        handleError("Please specify a port number between 1024 and 49151");
    }
    
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

        int* socketCopy = malloc(sizeof(int));  // Allocate memory for a copy of the socket descriptor
        *socketCopy = newSocket;  // Copy the socket descriptor value

        pthread_t requestHandlerThread;
        if (pthread_create(&requestHandlerThread, NULL, handleRequest, socketCopy) != 0) {
            handleError("Error when creating thread");
        }

    }
    
    return 0;
}