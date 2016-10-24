#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>


#include "encode.h"
#include "decode.h"
#include "delta.h"
#include "runLength.h"
#include "huffman.h"
#include "fileResources.h"

/* ************************************************************************* */
/* Main */
/* ************************************************************************* */
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
            printf( "Requested delta enconding (%i) \n", options );
        }
        // Check for run-length encoding
        pCh = strstr( optionsInput, "-c" );
        if ( pCh != NULL ){
            options = (options | 0b00000010);
            printf( "Requested run-length encoding (%i) \n", options );
        }
        // Check for Huffman coding
        pCh = strstr( optionsInput, "-h" );
        if ( pCh != NULL ){
            options = (options | 0b00000001);
            printf( "Requested huffman encoding (%i) \n", options );
        }

        encode( inputFilename, outputFilename, options );
        printf( "File encoded to %s\n", outputFilename );
    }

    /* ------------------------------------------------------------------------- */
    /* Processing a decoding operation */

    if( strcmp( operationInput, "decode" ) == 0 ){
        decode( inputFilename, outputFilename );
    }

    return 0;
}
