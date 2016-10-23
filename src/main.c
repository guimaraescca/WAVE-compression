#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>

/* Struct that defines the header data of a WAVE file */
typedef struct waveHeader {
    char chunkID[4];
    int chunckSize;
    char format[4];

    char subChunk1ID[4];
    int subChunk1Size;
    short int audioFormat;
    short int numChannels;
    int sampleRate;
    int byteRate;
    short int blockAlign;
    short int bitsPerSample;    // Offset 34

    char subChunck2ID[4];
    int subChunk2Size;          // Number of samples
} waveHeader;

typedef enum {
    false, true
} bool;

/* Returns the compression header
       The compression header will be included on the generated output file, it indicates the compression methods used.
       The header is a 4 byte component that precedes the compressed data.
*/
int getCompHeader( char* filename ) {

    FILE *inputFile = fopen(filename, "r");
    char buffer[10];
    char* ptr;
    int header;

    if( inputFile == NULL ) {
        printf( "[Error] \tUnable to read input file. <getCompHeader>\n" );
        return 0;
    }

    fread( buffer, 1, 1, inputFile );   // Read file header
    header = strtol(buffer, &ptr, 10);  // Transform file header to an int

    printf( "Compression header - as <char>: %s as <int>: %i\n", buffer, header );

    fclose( inputFile );
    return header;
}

int fileWrite( char* filename , char* options ) {

    FILE *write = fopen( filename, options );
    uint32_t number = 0b00001000;
    char header[1];

    snprintf( header, 1, "%d", number );

    printf( "Writing number: %i as a string: %s\n", number, header );

    fwrite( header, sizeof(char), sizeof(header), write );

    fclose( write );
    return 0;
}

/* Delta encoding function */
int deltaEncode( int* sampleArrayInt, waveHeader* header ){

    int* deltaArray = ( int* )malloc( header->subChunk2Size*sizeof( int ) ); // Differrences array
    int i;
    int minimumDelta = 0;   // Used to give a better compression rate

    deltaArray[0] = sampleArrayInt[0];

    for( i=1; i<header->subChunk2Size; i++ ){

        deltaArray[i] = sampleArrayInt[i] - sampleArrayInt[i-1];

        if ( deltaArray[i] < minimumDelta){

            minimumDelta = deltaArray[i];
        }
    }
    return 1;
}

/* Delta decoding function */
int deltaDecode( int* sampleArrayInt, waveHeader* header ){

}

/* Run-length encoding function */
int* runLengthEncode( int* sampleArrayInt, int arraySize , int* runLengthSize ){

    // By design assumes that the sequence always starts with 0's
    int i, j;
    int bit, actualCount = 0;
    int count = 0;
    int* runLengthArray = 0;
    int runLengthArraySize = 0;

    for( i=0; i<arraySize; i++ ){

        for(j=31; j>=0; j--){

            bit = ( sampleArrayInt[i] >> j ) & 1;

            if( bit == actualCount ){

                count++;

            }else{

                runLengthArraySize++;
                runLengthArray = ( int* )realloc( runLengthArray, runLengthArraySize*sizeof( int ) );   // Increments the runLengthArray 1 by 1 element
                runLengthArray[ runLengthArraySize-1 ] = count;
                count = 1;
                actualCount = bit;
            }
        }
    }
    *runLengthSize = runLengthArraySize;
    return runLengthArray;
}

int huffmanEncode( int* sampleArrayInt, int arraySize ){

}

/*  sampleArray - Char array containing representing 1 sample
    size - Number of bits per sample
    intSample - Value where the sample will be computed into an int
    */
int recursiveCharToIntSample( char* sampleArray, int size, int intSample ){

    intSample = intSample << ( 8 );

    intSample = intSample | (((int)sampleArray[0]) & 0x000000ff );

    //memmove( sampleArray, sampleArray+1, strlen( sampleArray ) );

    if( size == 8 ){
        return intSample;
    }

    recursiveCharToIntSample( sampleArray+1, size-8, intSample );
}

/* Encode processing */
int encode( char* inputFilename, char* outputFilename, uint32_t options ) {

    FILE* inputFile = fopen( inputFilename, "rb" );
    waveHeader* header = ( waveHeader* )malloc( sizeof( waveHeader ) );
    int charRead;
    int nCharRead = 0;
    int charPerSample;
    int charRequired;

    char* sampleArray;
    int* sampleArrayInt;
    char charToIntArray[(header->bitsPerSample/8)-1];

    if( inputFile == NULL ) {
        printf( "[Error] \tUnable to read input file. <encode>\n" );
        return 0;
    }

    fread( header, 1, sizeof( waveHeader ), inputFile );

    charPerSample = ( header->bitsPerSample/8 );        // Number of chars required to store 1 sample
    charRequired = charPerSample*header->subChunk2Size; // Number of chars required to store the file

    sampleArray = ( char* )malloc( charRequired*sizeof( char ) );           // Allocate the array used to store the samples as char
    sampleArrayInt = ( int* )malloc( header->subChunk2Size*sizeof( int ) ); // Allocate the array used to store the char samples into int

    /* Reading the file data */
    fseek( inputFile, 44, SEEK_SET );
    fread( sampleArray, 1, charRequired, inputFile );

    /* Transform every (bitsPerSample/8) chars into 1 int */
    int number;
    nCharRead = 0;
    while ( nCharRead <= header->subChunk2Size ){

        memmove( charToIntArray, sampleArray + nCharRead, charPerSample );
        number = recursiveCharToIntSample(charToIntArray, header->bitsPerSample, 0);
        sampleArrayInt[ nCharRead/charPerSample ] = number;
        nCharRead = nCharRead + charPerSample;
    }

    /* Execute the encoding algorithms requested */
    int arraySize;
    if( (options & 0b00000100) == 0b00000100 ){ //Delta encoding

        deltaEncode( sampleArrayInt, header );
    }
    if( (options & 0b00000010) == 0b00000010 ){ //Run-length encoding

        int runLengthSize;
        int*  aux = runLengthEncode( sampleArrayInt, header->subChunk2Size , &runLengthSize);
        free(sampleArrayInt);
        sampleArrayInt = aux;       // Keeps encapsulation by sending the same array to the next encoding
        arraySize = runLengthSize;
    }
    if( (options & 0b00000001) == 0b00000001 ){ //Huffman encoding
        huffmanEncode(sampleArrayInt, arraySize);
    }
}

/* Decode processing */
int decode( char* inputFilename, char* outputFilename ) {

    printf( "Decoding file input '%s' to ouput '%s'\n", inputFilename, outputFilename );
    getCompHeader( inputFilename );
}

int main( int argc, char* argv[] ) {

    int i;
    uint32_t options = 0b00000000;
    char operationInput[50];
    char optionsInput[50];
    char inputFilename[50];
    char outputFilename[50];
    char* pCh;

    //fileWrite( argv[2], "w+" );

    /* Processing input */
    if( argc < 3 ){
        printf( "[Error] \tInsuficient number of inputs(%i).\n", argc );
        return 0;
    }
    strcpy(operationInput, argv[1]);
    strcpy(inputFilename, argv[argc-2]);
    strcpy(outputFilename, argv[argc-1]);

    printf( "Operation: %s %s %s\n", operationInput, inputFilename, outputFilename );

    /* Processing a code operation */
    if ( strcmp(operationInput,"encode") == 0 ){

        if ( argc <= 4 ){
            printf( "[Error] \tInsuficient number of inputs (%i). \nTry again using: encode (-d | -c | -h) <input.wav> <output.bin>\n", argc );
            return 0;
        }

        /* Concatenate all inputs in argv, to identify the options */
        for( i = 2; i <= (argc-3); i++ ){   // If we have more than 4 argc inputs it's ok to do it
            strcat( optionsInput, argv[i] );
        }

        // Check for differences encoding
        pCh = strstr( optionsInput, "-d" );
        if ( pCh != NULL ){
            options = (options | 0b00000100);
            printf( "Requested -d (%i) \n", options );
        }
        // Check for run-length encoding
        pCh = strstr( optionsInput, "-c" );
        if ( pCh != NULL ){
            options = (options | 0b00000010);
            printf( "Requested -c (%i) \n", options );
        }
        // Check for Huffman coding
        pCh = strstr( optionsInput, "-h" );
        if ( pCh != NULL ){
            options = (options | 0b00000001);
            printf( "Requested -h (%i) \n", options );
        }

        encode( inputFilename, outputFilename, options );
    }

    /* Processing a decoding operation */
    if( strcmp( operationInput, "decode" ) == 0 ){
        decode( inputFilename, outputFilename );
    }

    return 0;
}
