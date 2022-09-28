CC=cc
CFLAGS=-Wall -Wextra -Werror -g
OBJECTS=client.o server.o filetransfer.o
TARGET=a.out

all: ${TARGET}

client.o: client.c filetransfer.h
	${CC} ${CFLAGS} -c client.c -o client.o

server.o: server.c filetransfer.h
	${CC} ${CFLAGS} -c server.c -o server.o

filetransfer.o: filetransfer.c
	${CC} ${CFLAGS} -c filetransfer.c -o filetransfer.o

${TARGET}: ${OBJECTS}
	${CC} -g ${OBJECTS} -o ${TARGET}

clean:
	rm ${OBJECTS} ${TARGET}

.PHONY: all clean
