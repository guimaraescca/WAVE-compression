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

typedef struct compressionHeader {
    int originalSize;
    int options;
}compressionHeader;

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
int deltaDecode( int* sampleArrayInt, int arraySize ){


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

/* Run-length decoding function */
int* runLengthDecode( int* sampleArrayInt, int arraySize , int* runLengthSize ){

    int i, j;
    int totalCount = 0;

    for(i=0; i<=arraySize; i++){
        totalCount = totalCount + sampleArrayInt[i];
    }

    int newArraySize = ceil( (double)totalCount/32.0 ) ;
    int* newSampleArray = ( int* )calloc(newArraySize, sizeof(int) );
    int bitCount = 0;
    int writingBit = 0;

    for( i=0; i< arraySize; i++ ){
        for( j=0; j<sampleArrayInt[i]; j++ ){
            // Generating bits from run-length decoding and writing them as a new array
            newSampleArray[ bitCount/32 ] = newSampleArray[ bitCount/32 ] | ( writingBit << ( 31-( bitCount%32 ) ) );
            bitCount++;
        }
        writingBit = !writingBit;       // Starts counting the next sequence
    }

    *runLengthSize = newArraySize;
    return newSampleArray;
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

/* ************************************************************************* */
/* Encode processing */
/* ************************************************************************* */
int encode( char* inputFilename, char* outputFilename, uint32_t options ) {

    FILE* inputFile = fopen( inputFilename, "rb" );
    waveHeader* header = ( waveHeader* )malloc( sizeof( waveHeader ) );
    int charRead;
    int nCharRead = 0;

    char charToIntArray[(header->bitsPerSample/8)-1];

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

    int beginLastSubChunk = sizeof( waveHeader ) + header->subChunk2Size;
    fseek( inputFile, 0, SEEK_END );
    int endLastSubChunk = ftell( inputFile );
    int lastSubChunkSize = endLastSubChunk - beginLastSubChunk;

    char* lastSubChunk = 0;
    if( lastSubChunkSize !=0 ){

        lastSubChunk = ( char* )malloc( lastSubChunkSize*sizeof( char ) );
        fread( lastSubChunk, 1, lastSubChunkSize, inputFile);
    }

    /* ------------------------------------------------------------------------- */
    /* Transform every 'bitsPerSample/8' chars into 1 int */

    int number;
    nCharRead = 0;
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

}

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

    int charPerSample = ( header->bitsPerSample/8 );        // Number of chars required to store 1 sample
    int charRequired = charPerSample*header->subChunk2Size; // Number of chars required to store the file

    char* sampleArray = ( char* )malloc( charRequired*sizeof( char ) );           // Allocate the array used to store the samples as char
    int* sampleArrayInt = ( int* )malloc( compHeader->originalSize ); // Allocate the array used to store the char samples into int

    fseek( inputFile, ( sizeof( compressionHeader ) + sizeof( waveHeader ) ), SEEK_SET );
    fread( sampleArray, 1, compHeader->originalSize, inputFile );

    /* ------------------------------------------------------------------------- */
    /* Reading data after the audio data */

    int beginLastSubChunk = sizeof( waveHeader ) + sizeof( compHeader ) + compHeader->originalSize;
    fseek( inputFile, 0, SEEK_END );
    int endLastSubChunk = ftell( inputFile );
    int lastSubChunkSize = endLastSubChunk - beginLastSubChunk;

    if( lastSubChunkSize !=0 ){

        char* lastSubChunk = ( char* )malloc( lastSubChunkSize*sizeof( char ) );
        fread( lastSubChunk, 1, lastSubChunkSize, inputFile);
    }

    /* ------------------------------------------------------------------------- */
    /* Execute the decoding algorithms in the compression header */

    int arraySize = compHeader->originalSize;
    if( ( compHeader->options & 0b00000100) == 0b00000100 ){ //Delta decoding

        deltaDecode( sampleArrayInt, arraySize );
    }
    if( ( compHeader->options & 0b00000010) == 0b00000010 ){ //Run-length decoding

        int runLengthSize;
        int* aux = runLengthDecode( sampleArrayInt, arraySize , &runLengthSize );
        free( sampleArrayInt );
        sampleArrayInt = aux;
        arraySize = runLengthSize;
    }
    if( ( compHeader->options & 0b00000001) == 0b00000001 ){ //Huffman decoding

    }
}

int main( int argc, char* argv[] ) {

    int i;
    uint32_t options = 0b00000000;
    char operationInput[50];
    char optionsInput[50];
    char inputFilename[50];
    char outputFilename[50];
    char* pCh;

    /* ------------------------------------------------------------------------- */
    /* Processing input */

    if( argc < 3 ){
        printf( "[Error] \tInsuficient number of inputs(%i).\n", argc );
        return 0;
    }

    strcpy(operationInput, argv[1]);
    strcpy(inputFilename, argv[argc-2]);
    strcpy(outputFilename, argv[argc-1]);

    printf( "Operation: %s %s %s\n", operationInput, inputFilename, outputFilename );

    /* ------------------------------------------------------------------------- */
    /* Processing a coding operation */

    if ( strcmp(operationInput,"encode") == 0 ){

        if ( argc <= 4 ){
            printf( "[Error] \tInsuficient number of inputs (%i). \nTry again using: encode (-d | -c | -h) <input.wav> <output.bin>\n", argc );
            return 0;
        }

        /* ------------------------------------------------------------------------- */
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

    /* ------------------------------------------------------------------------- */
    /* Processing a decoding operation */

    if( strcmp( operationInput, "decode" ) == 0 ){
        decode( inputFilename, outputFilename );
    }

    return 0;
}
