#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>

#include "runLength.h"
#include "fileResources.h"

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

    printf("GOKU arraysize %i \n", arraySize );
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
