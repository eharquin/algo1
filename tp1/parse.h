#ifndef PARSE_H
#define PARSE_H

#define MAX_SEQ_LENGTH 1024
#define MAX_STRING_LENGTH 128

/*
 * Read the next test header (identifier and function name).
 * Return 1 if successful, 0 if there is no more test (EOF)
 */
int read_test_header(char test_id[], char test_function[]);

void read_int(int *num);
int read_int_array(int arr[], int size);

void read_string(char str[]);

#endif /* PARSE_H */
