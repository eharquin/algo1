#include "arrays.h"
#include <stdlib.h>

int *create_array(int max_size) {
    int *ptr = (int*)malloc(max_size*sizeof(int));
    return ptr;
}

void free_array(int t[]) {
    free(t);
}

void insert_unsorted(int t[], int *size, int elt) {
    t[*size] = elt;
    (*size)++;
}

int find_unsorted(int t[], int size, int elt) {
    int i;
    for (i = 0; i < size; i++)
        if (t[i] == elt)
            return 1;
    return 0;
}

void insert_sorted(int t[], int *size, int elt) {
    int j = *size;
    while (j > 0 && elt < t[j-1]) {
        t[j] = t[j-1];
        j--;
    }
    t[j] = elt;
    (*size)++;
}

int find_sorted(int t[], int size, int elt) {
    int low = 0;
    int mid;
    int high = size-1;
    int cmp;
    while (low <= high) {
        mid = (high+low)/2;
        cmp = t[mid]-elt;
        if (cmp == 0) 
            return 1;
        else if (cmp < 0)
            low = mid+1;
        else
            high = mid-1;
    }
    return 0;
}
