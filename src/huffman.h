#ifndef HUFFMAN_H
#define HUFFMAN_H

typedef struct huffman_node{
    int number;
    int frequency;
    struct huffman_node *left;
    struct huffman_node *right;
    int isLeaf;
}huffman_node;

typedef struct huffman_tree{
    struct huffman_node *left;
    struct huffman_node *right;
}huffman_tree;

int quicksort( int*, int, int );

int quicksortDivide( int*, int, int );

int huffman_quicksortDivide( huffman_node** , int , int );

int huffman_quicksort( huffman_node** , int , int );

huffman_node* huffman_createNode( int, int );

int huffman_printNodes( huffman_node**, int );

huffman_node* generateTree( huffman_node** , int );

huffman_node** getFrequency( int*, int, int* );

int huffmanEncode( int*, int );

#endif
