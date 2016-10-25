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

    FILE* inputFile = fopen( inputFilename, "r" );

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

    int* decodeArray = ( int* )malloc( compHeader->originalSize*sizeof(int) );      /* Allocate the array used to store the data as integers */
    fread( decodeArray, 1, compHeader->originalSize*sizeof(int), inputFile );

    /* ------------------------------------------------------------------------- */
    /* Reading the subchuncks after the audio data */

    int lastSubChunkSize = ( getFileSize( inputFile ) - ( sizeof( compressionHeader ) + sizeof( waveHeader ) + compHeader->originalSize*sizeof( int ) ) );
    char* lastSubChunk = ( char* )malloc( lastSubChunkSize*sizeof( char ) );

    fread( lastSubChunk, 1, lastSubChunkSize, inputFile);

    /* ------------------------------------------------------------------------- */
    /* Execute the decoding algorithms in the compression header */

    int arraySize = compHeader->originalSize;

    /* Delta decoding */
    if( ( compHeader->options & 0b00000100) == 0b00000100 ){

        deltaDecode( decodeArray, arraySize );

    }

    /* Run-length decoding */
    if( ( compHeader->options & 0b00000010) == 0b00000010 ){

        int runLengthSize;
        int* aux = runLengthDecode( decodeArray, arraySize , &runLengthSize );
        free( decodeArray );
        decodeArray = aux;
        arraySize = runLengthSize;

    }

    /* Huffman decoding */
    if( ( compHeader->options & 0b00000001) == 0b00000001 ){

    }
    /* ------------------------------------------------------------------------- */
    /* Transform every int into 'bitsPerSample/8' char */

    int i, j, number, count = 0;
    char decodeArrayChar[ arraySize*(header->bitsPerSample/8) ];

    for( i = 0;  i < arraySize; i++ ){

        number = 0;

        for( j = 0; j < ( header->bitsPerSample/8 ); j++ ){

            number = decodeArray[i] >> ( ( ( header->bitsPerSample/8 ) - j - 1 ) * 8 );
            decodeArrayChar[count] = (char)number;
            count++;

        }
    }

    FILE* outputFile = fopen( outputFilename, "w" );

    fwrite( header, sizeof( waveHeader ), 1, outputFile );
    fwrite( decodeArrayChar, compHeader->originalSize*(header->bitsPerSample/8), 1, outputFile);
    fwrite( lastSubChunk, lastSubChunkSize, 1, outputFile );

    return 1;
}
