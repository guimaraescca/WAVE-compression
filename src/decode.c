#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>

#include "delta.h"
#include "runLength.h"
#include "huffman.h"
#include "fileResources.h"

/* ************************************************************************* */
/* Decode processing */
/* ************************************************************************* */
int decode( char* inputFilename, char* outputFilename ) {

    FILE* inputFile = fopen( inputFilename, "rb" );

    waveHeader* header = ( waveHeader* )malloc( sizeof( waveHeader ) );
    compressionHeader* compHeader = ( compressionHeader* )malloc( sizeof( compressionHeader ) );

    if( inputFile == NULL ) {
        printf( "[Error] \tUnable to read input file. <decode>\n" );
        return 0;
    }

    fread( compHeader, 1, sizeof( compressionHeader ), inputFile );
    fread( header, 1, sizeof( waveHeader ), inputFile );

    /* ------------------------------------------------------------------------- */
    /* Reading the file data */

    //int charPerSample = ( header->bitsPerSample/8 );        // Number of chars required to store 1 sample
    //int charRequired = charPerSample*header->subChunk2Size; // Number of chars required to store the file

    int* decodeArray = ( int* )malloc( compHeader->originalSize*sizeof(int) );      // Allocate the array used to store the data as integers

    fread( decodeArray, 1, compHeader->originalSize*sizeof(int), inputFile );

    /* ------------------------------------------------------------------------- */
    /* Reading data after the audio data */

    int beginLastSubChunk = sizeof( waveHeader ) + sizeof( compHeader ) + compHeader->originalSize;
    //fseek( inputFile, 0, SEEK_END );
    //int endLastSubChunk = ftell( inputFile-1 );
    int endLastSubChunk = getFileSize(inputFilename);
    int lastSubChunkSize = endLastSubChunk - beginLastSubChunk;

    char* lastSubChunk = 0;
    if( lastSubChunkSize !=0 ){

        lastSubChunk = ( char* )malloc( lastSubChunkSize*sizeof( char ) );
        fread( lastSubChunk, 1, lastSubChunkSize, inputFile);
    }

    /* ------------------------------------------------------------------------- */
    /* Execute the decoding algorithms in the compression header */

    int arraySize = compHeader->originalSize;
    if( ( compHeader->options & 0b00000100) == 0b00000100 ){ //Delta decoding

        deltaDecode( decodeArray, arraySize );
    }
    if( ( compHeader->options & 0b00000010) == 0b00000010 ){ //Run-length decoding

        int runLengthSize;
        int* aux = runLengthDecode( decodeArray, arraySize , &runLengthSize );
        free( decodeArray );
        decodeArray = aux;
        arraySize = runLengthSize;
    }
    if( ( compHeader->options & 0b00000001) == 0b00000001 ){ //Huffman decoding

    }

    FILE* outputFile = fopen( outputFilename, "wb" );
    fwrite( header, sizeof( waveHeader ), 1, outputFile );
    fwrite( decodeArray, arraySize, 1, outputFile);
    fwrite( lastSubChunk, lastSubChunkSize, 1, outputFile );
}
