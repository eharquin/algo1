#include "verif_func.h"
#include "arrays.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int tab[MAX_SEQ_LENGTH];
static int tab_exp[MAX_SEQ_LENGTH];

/*
 * Return -1 if the arrays are equal.
 * Otherwise, return the first index for they differ.
 */
int index_diff(int *t1, int size1, int *t2, int size2) {
    if (size1 != size2) return 0;
    int i;
    for (i = 0; i < size1; i++)
        if (t1[i] != t2[i])
            return i;
    return -1;
}

void display_array(int *t, int size) {
    int i = 0;
    printf("[");
    for (i = 0; i < size; i++) {
        printf("%d",t[i]);
        if (i < size-1) printf(", ");
    }
    printf("]\n");
}

void display_value_error(char *str, int exp, int rec) {
    printf("\033[1m\033[91mERROR\033[0m: %s\n", str);
    printf("Expected: %d\n", exp);
    printf("Received: %d\n", rec);
}

void display_array_error(char *str, int *tab_exp, int size_exp, int *tab_rec, int size_rec) {
    printf("\033[1m\033[91mERROR\033[0m: %s\n", str);
    printf("Expected: "); display_array(tab_exp, size_exp);
    printf("Received: "); display_array(tab_rec, size_rec);
}

void display_multi_error(char *str, int *tab, int size, int elt, int exp, int rec) {
    printf("\033[1m\033[91mERROR\033[0m: %s\n", str);
    printf("Find (%d) in array: ", elt); display_array(tab, size);
    printf("Expected: %d\n", exp);
    printf("Received: %d\n", rec);
}

int verify_insert_compare(int size, int size_exp) {

    int ok;

    ok = size == size_exp;

    if (!ok) {
        display_value_error("the size differs", size_exp, size);
        return 0;
    }

    ok = ok && (index_diff(tab, size, tab_exp, size_exp) == -1);
    
    if (!ok) {
        display_array_error("the array differs", tab_exp, size, tab, size);
        return 0;
    } 

    int idx = index_diff(tab, MAX_SEQ_LENGTH, tab_exp, MAX_SEQ_LENGTH);
    ok = ok && (idx == -1);

    if (!ok) {
        display_array_error("the memory differs outside of the array boundary", tab_exp, idx+1, tab, idx+1);
        return 0;
    }

    return ok; /* this should be 1 */
}

int verify_insert(void (*fn)(int*, int*, int)) {

    int size, elt;
    int size_exp;
    
    /* read test */
    read_int_array(tab, MAX_SEQ_LENGTH);
    read_int(&size);
    read_int(&elt);
    
    read_int_array(tab_exp, MAX_SEQ_LENGTH);
    read_int(&size_exp);

    /* run test */
    (*fn)(tab, &size, elt);

    return verify_insert_compare(size, size_exp);
}

int verify_insert_multi(void (*fn)(int*, int*, int)) {

    int size, elts[MAX_SEQ_LENGTH];
    int size_exp;
    
    /* read test */
    read_int_array(tab, MAX_SEQ_LENGTH);
    read_int(&size);
    int n_tests = read_int_array(elts, MAX_SEQ_LENGTH);
    
    read_int_array(tab_exp, MAX_SEQ_LENGTH);
    read_int(&size_exp);

    int i;
    for (i = 0; i < n_tests; i++) {

        /* run test */
        (*fn)(tab, &size, elts[i]);
    }

    return verify_insert_compare(size, size_exp);
}

int verify_find(int (*fn)(int*, int, int)) {

    int size, elt;
    int exp;
    int res;

    /* read test */
    read_int_array(tab, MAX_SEQ_LENGTH);
    read_int(&size);
    read_int(&elt);
    
    read_int(&exp);

    /* run test */
    res = (*fn)(tab, size, elt);

    int ok;

    ok = res == exp;

    if (!ok) {
        display_value_error("the return value differs", exp, res);
    }

    return ok;
}

int verify_find_multi(int (*fn)(int*, int, int)) {

    int size, elts[MAX_SEQ_LENGTH];
    int exp[MAX_SEQ_LENGTH];
    int res;
    int n_tests;

    /* read test */
    read_int_array(tab, MAX_SEQ_LENGTH);
    read_int(&size);
    n_tests = read_int_array(elts, MAX_SEQ_LENGTH);
    
    read_int_array(exp, MAX_SEQ_LENGTH);

    int i, ok;
    for (i = 0; i < n_tests; i++) {
        /* run test */
        res = (*fn)(tab, size, elts[i]);

        ok = (res == exp[i]);

        if (!ok) {
            display_multi_error("the return value differs", tab, size, elts[i], exp[i], res);
            return 0;
        }
    }

    return ok; /* this should be 1 */
}

int run_test(char test_id[], char test_function[]) {

    int res = -1;

    if (strcmp(test_function, "verify_insert_unsorted") == 0)
    {
        res = verify_insert(&insert_unsorted);
    } else if (strcmp(test_function, "verify_insert_unsorted_multi") == 0)
    {
        res = verify_insert_multi(&insert_unsorted);
    } else if (strcmp(test_function, "verify_find_unsorted") == 0)
    {
        res = verify_find(&find_unsorted);
    } else if (strcmp(test_function, "verify_find_unsorted_multi") == 0)
    {
        res = verify_find_multi(&find_unsorted);
    } else if (strcmp(test_function, "verify_insert_sorted") == 0)
    {
        res = verify_insert(&insert_sorted);
    } else if (strcmp(test_function, "verify_insert_sorted_multi") == 0)
    {
        res = verify_insert_multi(&insert_sorted);
    } else if (strcmp(test_function, "verify_find_sorted") == 0)
    {
        res = verify_find(&find_sorted);
    } else if (strcmp(test_function, "verify_find_sorted_multi") == 0)
    {
        res = verify_find_multi(&find_sorted);
    } else {
        fprintf(stderr, "Error in test case %s: test function %s does not exist\n", test_id, test_function);
        exit(1);
    }

    return res;
}
