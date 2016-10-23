#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>

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

    fread( buffer, 4, 1, inputFile );   // Read file header
    header = strtol(buffer, &ptr, 10);  // Transform file header to an int

    printf( "Compression header - as <char>: %s as <int>: %i\n", buffer, header );

    fclose( inputFile );
    return header;
}

int fileWrite( char* filename , char* options) {

    FILE *write = fopen( filename, options );
    uint32_t number = 0b00001000;
    char header[1];

    snprintf( header, 1, "%d", number );

    printf( "Writing number: %i as a string: %s\n", number, header );

    fwrite( header, sizeof(char), sizeof(header), write );

    fclose( write );
    return 0;
}

/* Encode processing */
int encode( char* inputFilename, char* outputFilename, uint32_t options ) {

    FILE* inputFile = fopen( inputFilename, "rb" );
    waveHeader* header = ( waveHeader* )malloc( sizeof( waveHeader ) );

    if( inputFile == NULL ) {
        printf( "[Error] \tUnable to read input file. <encode>\n" );
        return 0;
    }

    fread( header, 1, sizeof( waveHeader ), inputFile );

    printf( "[Encode]\tBits per sample: %i\n", header->bitsPerSample );
    printf( "[Encode]\tNumber of samples: %i\n", header->subChunk2Size );
/*
    while( !feof( inputFile ) ){
        // Reading
    }
*/
}

/* Decode processing */
int decode( char* inputFilename, char* outputFilename ) {

    printf( "Decoding file input '%s' to ouput '%s'\n", inputFilename, outputFilename );
    getCompHeader( inputFilename );
}

// TODO: what does it mean?
// char* pCh = ( optionsInput, "a" );

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
    if ( operationInput,"encode" ){

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
    if( operationInput, "decode" ){

        //decode( inputFilename, outputFilename );
    }

    return 0;
}
