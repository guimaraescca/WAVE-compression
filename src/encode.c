#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <sys/stat.h>

#include "delta.h"
#include "runLength.h"
#include "huffman.h"
#include "fileResources.h"

/* ************************************************************************* */
/* Encode processing */
/* ************************************************************************* */
int encode( char* inputFilename, char* outputFilename, uint32_t options ) {

    FILE* inputFile = fopen( inputFilename, "rb" );
    waveHeader* header = ( waveHeader* )malloc( sizeof( waveHeader ) );
    int charRead;
    int nCharRead = 0;

    if( inputFile == NULL ) {
        printf( "[Error] \tUnable to read input file. <encode>\n" );
        return 0;
    }

    fread( header, 1, sizeof( waveHeader ), inputFile );

    int charPerSample = ( header->bitsPerSample/8 );        // Number of chars required to store 1 sample
    int charRequired = charPerSample*header->subChunk2Size; // Number of chars required to store the file

    char* sampleArray = ( char* )malloc( charRequired*sizeof( char ) );           // Allocate the array used to store the samples as char
    int* sampleArrayInt = ( int* )malloc( header->subChunk2Size*sizeof( int ) ); // Allocate the array used to store the char samples into int

    /* ------------------------------------------------------------------------- */
    /* Reading the file data */

    fseek( inputFile, 44, SEEK_SET );
    fread( sampleArray, 1, charRequired, inputFile );

    /* ------------------------------------------------------------------------- */
    /* Reading data after the audio data */

    // Obtaining the size os the chunck afters the audio data
    int beginLastSubChunk = sizeof( waveHeader ) + header->subChunk2Size;
    fseek( inputFile, 0, SEEK_END );
    int endLastSubChunk = ftell( inputFile );
    int lastSubChunkSize = endLastSubChunk - beginLastSubChunk;

    // Reading the chunck
    fseek( inputFile, ( sizeof( waveHeader ) + charRequired*sizeof(char) ), SEEK_SET );
    char* lastSubChunk = 0;
    if( lastSubChunkSize !=0 ){

        lastSubChunk = ( char* )malloc( lastSubChunkSize*sizeof( char ) );
        fread( lastSubChunk, 1, lastSubChunkSize, inputFile);
    }

    /* ------------------------------------------------------------------------- */
    /* Transform every 'bitsPerSample/8' chars into 1 int */

    int number;
    nCharRead = 0;
    char charToIntArray[(header->bitsPerSample/8)];

    while ( nCharRead <= header->subChunk2Size ){

        memmove( charToIntArray, sampleArray + nCharRead, charPerSample );
        number = recursiveCharToIntSample(charToIntArray, header->bitsPerSample, 0);
        sampleArrayInt[ nCharRead/charPerSample ] = number;
        nCharRead = nCharRead + charPerSample;
    }

    /* ------------------------------------------------------------------------- */
    /* Execute the encoding algorithms requested */

    int arraySize;
    if( (options & 0b00000100) == 0b00000100 ){ //Delta encoding

        deltaEncode( sampleArrayInt, header );
    }
    if( (options & 0b00000010) == 0b00000010 ){ //Run-length encoding

        int runLengthSize;
        int*  aux = runLengthEncode( sampleArrayInt, header->subChunk2Size , &runLengthSize);
        free( sampleArrayInt );
        sampleArrayInt = aux;                   // Keeps encapsulation by sending the same array to the next encoding
        arraySize = runLengthSize;
    }
    if( (options & 0b00000001) == 0b00000001 ){ //Huffman encoding
        huffmanEncode(sampleArrayInt, arraySize);
    }

    /* ------------------------------------------------------------------------- */
    /* Writing encoded data to the output file */

    compressionHeader* compHeader = ( compressionHeader* )malloc( sizeof( compressionHeader ) );
    compHeader->originalSize = arraySize;
    compHeader->options = options;

    FILE* outputFile = fopen( outputFilename, "wb" );

    fwrite( compHeader, sizeof( compressionHeader ), 1, outputFile );
    fwrite( header, sizeof( waveHeader ), 1, outputFile );
    fwrite( sampleArrayInt, arraySize, 1, outputFile);
    fwrite( lastSubChunk, lastSubChunkSize, 1, outputFile );

    return 1;
}
