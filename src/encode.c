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
    char* sampleArray = ( char* )malloc( header->subChunk2Size*sizeof( char ) );    /* Allocates the array used to store the samples as char */
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

    /* Delta encoding */
    if( (options & 0b00000100) == 0b00000100 ){

        int* aux = deltaEncode( sampleArrayInt, arraySize );
        free( sampleArrayInt );
        sampleArrayInt = aux;       /* Keeps encapsulation by sending the same array to the next encoding function */

    }

    /* Run-length encoding */
    if( (options & 0b00000010) == 0b00000010 ){

        int runLengthSize;
        int*  aux = runLengthEncode( sampleArrayInt, arraySize, &runLengthSize);
        free( sampleArrayInt );
        sampleArrayInt = aux;
        arraySize = runLengthSize;

    }

    /* Huffman encoding */
    if( (options & 0b00000001) == 0b00000001 ){

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
    fclose(inputFile);

// ------------TESTES QUICKSORT-----------------------------

    quicksort( sampleArrayInt, 0, arraySize-1 );    /* Ordenates the samples array */

    int huffmanListSize;                            /* Stores the size computed by the 'getFrequency' function */
    huffman_node** list = getFrequency( sampleArrayInt, arraySize , &huffmanListSize );     /* Obtains the frequency associated to each sample */

    huffman_quicksort( list, 0, huffmanListSize-1 );

// ------------/TESTES QUICKSORT-----------------------------

// ------------TESTES TREE-----------------------------

    int vector[14] = {1,1,2,2,2,3,3,3,3,4,4,4,4,4};
    int vectorSize = 14;
    int vectorSizeHuffman;
    huffman_node** hlist = getFrequency( vector, vectorSize, &vectorSizeHuffman );
    //huffman_printNodes( hlist , vectorSizeHuffman );

    huffman_node* tree = generateTree( hlist, vectorSizeHuffman );
    //huffman_printNodes( hlist , vectorSizeHuffman );

// ------------/TESTES TREE-----------------------------


    return 1;
}
