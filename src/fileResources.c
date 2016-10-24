#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>

#include "fileResources.h"

/* Get file size */
off_t getFileSize(char *filename) {

    struct stat st;

    if ( stat( filename, &st ) == 0 ){
        return st.st_size;
    }
    return -1;
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
