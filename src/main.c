#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Struct that defines the header data of a WAVE file */
typedef struct waveHeader {
    char chunkID[4];
    int chunckSize;
    char format[4];s

    char subChunk1ID[4];
    int subChunk1Size;
    short int audioFormat;
    short int numChannels;
    int sampleRate;
    int byteRate;
    short int blockAlign;
    short int bitsPerSample;

    char subChunck2ID[4];
    int subChunk2Size;
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
    int number = 8;
    char header[4];

    snprintf( header, 4, "%d", number );

    printf( "Writing number: %i as a string: %s\n", number, header );

    fwrite( header, sizeof(char), sizeof(header), write );

    fclose( write );
    return 0;
}

/* Encode processing */
int encode( char* inputFilename, char* outputFilename) {

}

/* Decode processing */
int decode( char* inputFilename, char* outputFilename ) {

    printf( "Decoding file input '%s' to ouput '%s'\n", inputFilename, outputFilename );
    getCompHeader( inputFilename );
}

int main( int argc, char* argv[] ) {

    int i;
    char inputFilename[50];
    char outputFilename[50];
    char operationInput[10];
    char inputString[200];

    //fileWrite( argv[2], "w+" );

    /* Processing input */
    if( argc < 3 ){
        printf( "[Error] \tInsuficient number of inputs(%i).\n", argc );
        return 0;
    }

    printf( "Operation: %s %s %s\n", argv[1], argv[2], argv[3] );

    /* Processing a code operation */
    if ( operationInput,"encode" ){

        if ( argc <= 4 ){
            printf( "[Error] \tInsuficient number of inputs (%i). \nTry again using: encode (-d | -c | -h) <input.wav> <output.bin>\n", argc );
            return 0;
        }

        /* Concatenate all inputs in argv, to identify the options */
        for( i = 0; i < argc; i++ ){
            strcat( inputString, argv[i] );
        }

        //encode(inputFilename, outputFilename, argc);
    }

    /* Processing a decoding operation */
    if( operationInput, "decode" ){

        strcpy( inputFilename, argv[2] );
        strcpy( outputFilename, argv[3] );

        //decode(inputFilename, outputFilename);
    }

    return 0;
}
