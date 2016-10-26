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

    int i, response, validOp = 0;
    uint32_t options = 0b00000000;
    char operationInput[50];
    char optionsInput[50];
    char inputFilename[50];
    char outputFilename[50];
    char* pCh;

    strcpy(optionsInput,"");

    /* ------------------------------------------------------------------------- */
    /* Processing input */

    if( argc < 3 ){

        printf( "[Error] \tInsuficient number of inputs.\n" );

        return 0;

    }

    strcpy(operationInput, argv[1]);
    strcpy(inputFilename, argv[argc-2]);
    strcpy(outputFilename, argv[argc-1]);

    printf( "[Operation]\t%s %s %s\n", operationInput, inputFilename, outputFilename );

    /* ------------------------------------------------------------------------- */
    /* Processing a coding operation */


    if ( strcmp(operationInput,"encode") == 0 ){

        if ( argc <= 4 ){

            printf( "[Error] \tInsuficient number of inputs (%i). \nTry again using: encode (-d | -c | -h) <input.wav> <output.bin>\n", argc );

            return 0;

        }

        /* ------------------------------------------------------------------------- */
        /* Concatenate all inputs in argv, to identify the options */

        for( i = 2; i <= (argc-3); i++ ){   /* If we have more than 4 argc inputs it's ok to do it */

            strcat( optionsInput, argv[i] );    /* Concatenate all flags to better identify them */

        }

        /* Check for delta encoding request */
        pCh = strstr( optionsInput, "-d" );
        if ( pCh != NULL ){

            options = (options | 0b00000100);
            printf( "[Requested] \tDelta enconding \n" );

        }

        /* Check for run-length encoding request */
        pCh = strstr( optionsInput, "-c" );
        if ( pCh != NULL ){

            options = (options | 0b00000010);
            printf( "[Requested] \tRun-length encoding \n" );

        }

        /* Check for Huffman coding request */
        pCh = strstr( optionsInput, "-h" );
        if ( pCh != NULL ){

            options = (options | 0b00000001);
            printf( "[Requested] \tHuffman encoding \n" );

        }

        response = encode( inputFilename, outputFilename, options );

        if( response == 1 ){

            printf( "[Success]\tFile encoded to: %s\n", outputFilename );

        }

        free(pCh);

    }else{

        validOp = validOp + 1;          /* 'validOp = 1' means that the encode functionality was not requested */
    }

    /* ------------------------------------------------------------------------- */
    /* Processing a decoding operation */

    if( strcmp( operationInput, "decode" ) == 0 ){

        if ( argc <= 3 ){

            printf( "[Error] \tInsuficient number of inputs (%i). \nTry again using: decode <input.wav> <output.bin>\n", argc );
            return 0;
        }

        response = decode( inputFilename, outputFilename );

        if( response == 1 ){

            printf( "[Success]\tFile decoded to: %s\n", outputFilename );
        }

    }else{

        validOp = validOp + 2;          /* 'validOp = 2' means that the decode functionality was not requested */
    }

    if( validOp == 3 ){                 /* 'validOp = 3' means that either the encode and decode functionalities were not requested */

        printf( "[Error] \tOperation requested wasn't valid. (encode/decode) \nTry to use the syntax described on the README file.\n" );
    }

    return 0;

}
