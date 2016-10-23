CC=gcc
CFLAGS=-I
#DEPS=lib.h

all: ./src/main.c
	$(CC) -lm -o ./bin/main ./src/main.c

clean:
	rm -f ./bin/*.o ./bin/main