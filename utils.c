#include <string.h>
#include "utils.h"

char* getContentType(char* fileName, char* contentType){
    
    const char* dot = strrchr(fileName, '.'); // Find the last dot in the filename
    if (dot != NULL && dot[1] != '\0') {
        const char* filecontentType = dot + 1; // Point to the characters after the dot

        if (strcmp(filecontentType, "txt") == 0) {
            strncpy(contentType, "text/plain", 64);
        } else if (strcmp(filecontentType, "ico") == 0) {
            strncpy(contentType, "image/x-icon", 64);
        } else if (strcmp(filecontentType, "pdf") == 0) {
            strncpy(contentType, "application/pdf", 64);
        } else {
            strncpy(contentType, "application/octet-stream", 64);
        }
    } else {
        strncpy(contentType, "application/octet-stream", 64);
    }

    return contentType;
}