#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h> //for errors
#include <unistd.h> //for close
#include <stdlib.h>
#include "error.h"

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