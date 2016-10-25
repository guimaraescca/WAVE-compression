#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>

#include "fileResources.h"

int getFileSize(FILE *inFile){
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
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

int printCompHeader( compressionHeader* compHeader ){

    printf( "[Compression header] Original size: %i\n", compHeader->originalSize );
    printf( "[Compression header] Options: %i\n", compHeader->options );

    return 1;

}

int printWaveHeader( waveHeader* header ){

    printf("[Wave header] bitsPerSample: %i\n", header->bitsPerSample );
    printf("[Wave header] subChunk2Size: %i\n", header->subChunk2Size );

    return 1;
}
