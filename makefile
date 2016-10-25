CC=gcc
CFLAGS=-I
#DEPS=encode.h decode.h delta.h runLength.h huffman.h fileResources.h

all: main.o encode.o decode.o delta.o runLength.o huffman.o fileResources.o
	$(CC) -lm -o ./bin/main encode.o decode.o delta.o runLength.o huffman.o fileResources.o main.o

main.o:
	$(CC) -lm -c ./src/main.c

encode.o:
	$(CC) -lm -c ./src/encode.c

decode.o:
	$(CC) -lm -c ./src/decode.c

delta.o:
	$(CC) -lm -c ./src/delta.c

runLength.o:
	$(CC) -lm -c ./src/runLength.c

huffman.o:
	$(CC) -lm -c ./src/huffman.c

fileResources.o:
	$(CC) -lm -c ./src/fileResources.c

clean:
	rm -f ./bin/*.o ./bin/main ./*.o
