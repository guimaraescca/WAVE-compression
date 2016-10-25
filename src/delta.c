#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>

#include "delta.h"
#include "fileResources.h"

/* Delta encoding function */
int* deltaEncode( int* sampleArrayInt, int arraySize ){

    int* deltaArray = ( int* )malloc( arraySize*sizeof( int ) ); /* Store the differrences array */
    int i;

    deltaArray[0] = sampleArrayInt[0];

    for( i=1; i<arraySize; i++ ){

        deltaArray[i] = sampleArrayInt[i] - sampleArrayInt[i-1];
    }

    return deltaArray;
}

/* Delta decoding function */
int* deltaDecode( int* sampleArrayInt, int arraySize ){

    int* deltaArray = ( int* )calloc( arraySize, sizeof( int ) );
    int i;

    deltaArray[0] = sampleArrayInt[0];

    for( i=1; i<arraySize; i++ ){

        deltaArray[i] = sampleArrayInt[i-1] - sampleArrayInt[i];
    }

    return deltaArray;
}
