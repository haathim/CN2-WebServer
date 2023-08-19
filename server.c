#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //for sockaddr_in
#include <string.h>
#include <unistd.h> //for close
#include <errno.h> //for errors
#include <signal.h>
#include <pthread.h>

// #define PORT 18000
#define BACKLOG_QUEUE_LEN 10
#define NO_FLAGS 0
#define READ_BUFFER_SIZE 10000
#define RESOURCE_URL_SIZE 50
#define MAX_RESPONSE_SIZE 1000
#define NUM_OF_THREADS 5
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
        send404Error(client_socket);
        return;
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

    sendFileResponse(clientSocket, token+1);

    close(clientSocket);

    return NULL;
}

int main(int argc, char** argv){
    
    // check for arguments are valid
    // for now no command-line-arguments
    if (argc != 2){
        handleError("Please specify port to run as an argument");
    }
    

    int PORT = atoi(argv[1]);

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

        pthread_t requestHandlerThread;
        if (pthread_create(&requestHandlerThread, NULL, handleRequest, &newSocket) != 0) {
            handleError("Error when creating thread");
        }

    }
    
    return 0;
}