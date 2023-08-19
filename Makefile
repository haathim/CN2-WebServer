
SOURCE=server.c
BIN=server
FLAGS=-g -pthread

all: ${BIN}


${BIN}: ${SOURCE}
	gcc -o $@ $^ ${FLAGS}

clean: 
	rm -rf ${BIN}
