
SOURCE=server.c
BIN=server

all: server


@{BIN}: ${SOURCE}
	gcc -0 $@ $^

clean: 
	rm -rf ${BIN}