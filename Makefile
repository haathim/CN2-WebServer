
SOURCE=server.c utils.c error.c
BIN=server
FLAGS=-pthread
BIN_DBG=server-dbg
DBG_FLAGS=-g

all: ${BIN}


${BIN}: ${SOURCE}
	gcc -o $@ $^ ${FLAGS}

${BIN_DBG}: ${SOURCE}
	gcc -o $@ $^ ${FLAGS} ${DBG_FLAGS}

clean: 
	rm -rf ${BIN} ${BIN_DBG}
