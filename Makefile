
SOURCE=server.c
BIN=server

all: ${BIN}


${BIN}: ${SOURCE}
	gcc -o $@ $^ -g

clean: 
	rm -rf ${BIN}
