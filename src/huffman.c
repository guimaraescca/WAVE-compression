#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>

#include "huffman.h"
#include "fileResources.h"

/* Used by the Quicksort ordenation */
int quicksortDivide( int* array, int p, int r){
    int c = array[r];
    int k, t, j = p;

    for( k = p; k<r; k++ ){
        if( array[k] <= c ){

            t = array[j];
            array[j] = array[k];
            array[k] = t;
            ++j;
        }
    }

    t = array[j];
    array[j] = array[r];
    array[r] = t;

    return j;
}

/* Quicksort ordenation function used by the Huffman encoding */
int quicksort( int* array, int p, int r){

    int i;
    if( p<r ){

        i = quicksortDivide(array, p, r);
        quicksort( array, p, i-1  );
        quicksort( array, i+1, r  );

    }
}

int huffman_swapNodes( huffman_node* node1, huffman_node* node2 ){

    huffman_node* aux = huffman_createNode(0,0);

    aux->frequency = node1->frequency;
    aux->number = node1->number;

    node1->frequency = node2->frequency;
    node1->number = node2->number;

    node2->frequency = aux->frequency;
    node2->number = aux->number;

    free(aux);

}


/* Used by the Quicksort-Huffman ordenation of nodes */
int huffman_quicksortDivide( huffman_node** list, int st, int end){
    int c = ( list[end]->frequency );
    int k, j = st;

    for( k = st; k<end; k++ ){
        if( ( list[k]->frequency ) <= c ){                  /* Compares frequencies */

            huffman_swapNodes( list[j], list[k] );      /* Swap places between the nodes */
            ++j;
        }
    }

    huffman_swapNodes( list[j], list[end] );
    return j;
}

/* Quicksort ordenation function used by the Huffman encoding to ordenate the nodes by their frequency */
int huffman_quicksort( huffman_node** list, int p, int r){

    int i;
    if( p < r ){
    //printf("r: %i element: %i\n", r, list[22175]->frequency );
        i = huffman_quicksortDivide( list, p, r) ;
        huffman_quicksort( list, p, i-1  );
        huffman_quicksort( list, i+1, r  );
    }
}

huffman_node* huffman_createNode( int number, int frequency ){

    huffman_node* newNode = ( huffman_node* )malloc( sizeof( huffman_node ) );

    newNode->number = number;
    newNode->frequency = frequency;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;

}

/* Print the numbers and frequencies of the huffman_node's for debbuging purposes */
int huffman_printNodes( huffman_node** list , int listSize){

    int i = 0;
    huffman_node* node = NULL;

    if( listSize == 0 ){

        printf( "No valid huffman_node list.\n" );
        return 0;
    }

    for( i=0; i<listSize-1; i++ ){

        node = list[i];
        printf( "N: %i f: %i\n", node->number, node->frequency );
    }

    return 1;
}

/* Computates the frequency of the numbers in the array and store the numbers with the frequence in huffman_node's */
huffman_node** getFrequency( int* array, int arraySize , int* huffmanListSize ){

    int i;
    int count = 1;                  /* How many different numbers exist */

    huffman_node** orderedList = NULL;  /* List of ordered huffman_node's storing the numbers and frequencies */
    huffman_node* newNode = NULL;       /* Node to be inserted */

    for( i=0; i<arraySize; i++ ){

        if( ( i != 0 ) && ( array[i] == newNode->number ) ){    /* If we're not in the first element and repeated a number */

            ( newNode->frequency )++;

        }else{

            orderedList = ( huffman_node** )realloc( orderedList, count*sizeof( huffman_node* ) );   /* Reallocates memory to store the new array of huffman_node's */

            newNode = huffman_createNode( array[i], 1 ); /* Store both the number and reset the frequency */

            orderedList[count-1] = newNode;             /* Insert's the node in the ordered array */


            count++;                                    /* Next number read is different from the previous one */
        }
    }

    *huffmanListSize = (count-1);

    //newNode = NULL;                                     /* Desalocating memory */
    //free(newNode);

    return orderedList;
}

int huffmanEncode( int* sampleArrayInt, int arraySize ){
    return 0;
}

int huffmanDecode( int* sampleArrayInt, int arraySize ){
    return 0;
}
