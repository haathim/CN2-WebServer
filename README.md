# Minimalist Webserver

## Description

This is a minimalist webserver which supports responding to basic GET requests for `txt`, `ico` & `pdf` files. It supports handling concurrent requests using multithreading. 


## Build & Run

1. First add the content that you want to have the ability of serving to the `/content` directory.

1. Run the following commands on the terminal:
```bash
make
./server <port-you-want-to-run-on>
``` 

### Default Response
The server provides a default response, when no url parameters are passed. This response is stored int the `hello.txt` file. Please change if needed.
## Debugging
If you want to run the server in a debugging session,

1. Clone repo
```bash
git clone https://github.com/haathim/CN2-WebServer.git
cd CN2-WebServer
#checkout to correct branch (if not already)
git checkout multithread
```

1. Build the debug binary
```bash
make server-dbg
```
2. Run GDB
```bash
gdb ./server-dbg
#set neccassary breakpoints
(gdb) run <port-you-want-to-run-on> 
```

## Notes
Please note that is not a webserver developed for production use cases. 