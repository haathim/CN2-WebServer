#ifndef ERROR_H
#define ERROR_H

#define handleError(errMsg) {\
        fprintf(stderr, "%s\nError cause:  %d: %s\n", errMsg, errno, strerror(errno));\
        exit(1);\
    }

void send404Error(int clientSocket);

#endif