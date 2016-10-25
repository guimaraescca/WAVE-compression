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

    FILE* inputFile = fopen( inputFilename, "r" );
    waveHeader* header = ( waveHeader* )malloc( sizeof( waveHeader ) );
    int charRead;

    if( inputFile == NULL ) {

        printf( "[Error] \tUnable to read input file. <encode>\n" );
        return 0;

    }

    fread( header, 1, sizeof( waveHeader ), inputFile );

    int charPerSample = ( header->bitsPerSample/8 );        /* Number of chars required to store 1 sample */

    char* sampleArray = ( char* )malloc( header->subChunk2Size*sizeof( char ) );           /* Allocate the array used to store the samples as char */
    int* sampleArrayInt = ( int* )malloc( ( ( header->subChunk2Size*8 )/header->bitsPerSample )*sizeof( int ) ); /* Allocate the array used to store the char samples into int */

    /* ------------------------------------------------------------------------- */
    /* Reading the file data */

    fseek( inputFile, 44, SEEK_SET );
    fread( sampleArray, 1, header->subChunk2Size, inputFile );

    /* ------------------------------------------------------------------------- */
    /* Reading the subchunk after the audio data */

    int lastSubChunkSize = ( getFileSize( inputFile ) - ( sizeof( waveHeader ) + header->subChunk2Size ) );
    char* lastSubChunk = ( char* )malloc( lastSubChunkSize*sizeof( char ) );

    fread( lastSubChunk, 1, lastSubChunkSize, inputFile );

    /* ------------------------------------------------------------------------- */
    /* Transform every 'bitsPerSample/8' chars into 1 int */

    int i, j, number;
    char numberAsChar[header->bitsPerSample/8];

    for( i = 0;  i < header->subChunk2Size; i = i + ( header->bitsPerSample/8 ) ){

        for( j = 0; j < ( header->bitsPerSample/8 ); j++ ){

            numberAsChar[j] = sampleArray[i+j];

        }

        number = recursiveCharToIntSample( numberAsChar, header->bitsPerSample, 0 );

        sampleArrayInt[ i/( header->bitsPerSample/8 ) ] = number;

    }

    /* ------------------------------------------------------------------------- */
    /* Execute the encoding algorithms requested */

    int arraySize = ( ( header->subChunk2Size*8 )/header->bitsPerSample );

    if( (options & 0b00000100) == 0b00000100 ){ /* Delta encoding */

        deltaEncode( sampleArrayInt, header );

    }

    if( (options & 0b00000010) == 0b00000010 ){ /* Run-length encoding */

        int runLengthSize;
        int*  aux = runLengthEncode( sampleArrayInt, arraySize, &runLengthSize);
        free( sampleArrayInt );
        sampleArrayInt = aux;                   /* Keeps encapsulation by sending the same array to the next encoding function */
        arraySize = runLengthSize;

    }

    printf("en arraySize:%i\n array[i:213102]: %i array[i:213103]: %i\n", arraySize, sampleArrayInt[213102], sampleArrayInt[213103] );

    if( (options & 0b00000001) == 0b00000001 ){ /* Huffman encoding */

        huffmanEncode(sampleArrayInt, arraySize);

    }

    /* ------------------------------------------------------------------------- */
    /* Writing encoded data to the output file */

    compressionHeader* compHeader = ( compressionHeader* )malloc( sizeof( compressionHeader ) );
    compHeader->originalSize = arraySize;
    compHeader->options = options;

    FILE* outputFile = fopen( outputFilename, "w" );

    fwrite( compHeader, sizeof( compressionHeader ), 1, outputFile );
    fwrite( header, sizeof( waveHeader ), 1, outputFile );
    fwrite( sampleArrayInt, arraySize*sizeof(int), 1, outputFile);
    fwrite( lastSubChunk, lastSubChunkSize, 1, outputFile );

    fclose(outputFile);

// ----TESTE DE LEITURA------
    FILE* out = fopen( outputFilename, "r" );

    waveHeader* wheader = ( waveHeader* )malloc( sizeof( waveHeader ) );
    compressionHeader* cheader = ( compressionHeader* )malloc( sizeof( compressionHeader ) );

    fread( cheader, sizeof( compressionHeader ), 1, out );
    fread( wheader, sizeof( waveHeader ), 1, out );

    printWaveHeader(wheader);
    printCompHeader(cheader);

    int* decodeArray = ( int* )malloc( cheader->originalSize*sizeof(int) );      // Allocate the array used to store the data as integers
    fread( decodeArray, cheader->originalSize*sizeof(int), 1, out );

    printf("decodeArray[213103]: %i\n",  decodeArray[213103]);
//---------------
    fclose(inputFile);
    return 1;
}
