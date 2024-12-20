#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define PARSE_TRACE

typedef enum {
    STATE_FindStartOfToken,
    STATE_ParseNumber,
    STATE_ParseString,
    STATE_ParseQuotedString,
    STATE_Escape,
    STATE_EOF
} ParserState;

typedef enum {
    TOKEN_None,
    TOKEN_String,
    TOKEN_Number,
    TOKEN_StartOfSequence,
    TOKEN_Delimiter,
    TOKEN_EndOfSequence,
    TOKEN_End
} token_t;

#define MAX_TOKEN_LENGTH 1024

static char current_token[MAX_TOKEN_LENGTH];
static int token_pos = 0;
static char buffer_char;
static int buffer_size = 0;
static int lineno = 1;

static ParserState state = STATE_FindStartOfToken;

extern FILE *fh; /* file handle */

void parse_error(char *error_string) {
    fprintf(stderr, "ERROR in test file line %d: %s\n", lineno, error_string);
    exit(-1);
}

token_t read_token();

void read_int(int *num) {
    if (read_token() != TOKEN_Number)
        parse_error("Expected number");
    sscanf(current_token, "%d", num);
}

int read_int_array(int arr[], int max_size) {

    /* read allocation size */
    /* read_int(size); */

    /* allocate array with 0s */
    /* *arr = (int *)calloc(*size, sizeof(int)); */
    
    memset(arr, 0, max_size*sizeof(int));

    /* read array content */
    if (read_token() != TOKEN_StartOfSequence)
        parse_error("Expected start of sequence");

    /* read first number or empty sequence */
    token_t tok = read_token();
    if (tok == TOKEN_EndOfSequence) return 0;
    if (tok != TOKEN_Number)
        parse_error("Expected number or ']' in sequence");

    int i = 0;
    sscanf(current_token, "%d", &arr[i++]);

    while (1) {
        tok = read_token();
        if (tok == TOKEN_EndOfSequence) break;
        if (tok != TOKEN_Delimiter)
            parse_error("Expected ',' or ']' in sequence");

        if (i >= max_size)
            parse_error("Sequence length exceeds declated allocation size");

        read_int(&arr[i++]);
    }

    return i;
}

void read_string(char str[]) {
    if (read_token() != TOKEN_String)
        parse_error("Expected string");
    strcpy(str, current_token);
}

int read_test_header(char test_id[], char test_function[]) {
    token_t tok = read_token();
    if (tok != TOKEN_String && tok != TOKEN_End)
        parse_error("Expected test identifier");
    if (tok == TOKEN_End) {
        test_id[0] = '\0';
        test_function[0] = '\0';
        return 0;
    }
    strcpy(test_id, current_token);
    read_string(test_function);
    return 1;
}

token_t read_token() {

    if (state == STATE_EOF)
        return TOKEN_End;

    token_pos = 0;

    while (1) {

        if (token_pos >= MAX_TOKEN_LENGTH)
            parse_error("Token length exceed maximum token length");

        int c;
        if (buffer_size == 0) {
            c = fgetc(fh);
        } else {
            c = buffer_char;
            buffer_size = 0;
        }

#ifdef PARSE_TRACE
        printf("char, state _%c_%d_\n", c, state);
#endif

        switch (state) {
        case STATE_FindStartOfToken:
            if (c == EOF) return TOKEN_End;
            if (c == '\n') { lineno++; break; }
            if (c == ' ' || c == '\t') break;
            if (c == '#') {
                /* ignore input until end of line of file */
                while ((c = fgetc(fh)) != EOF)
                    if (c == '\n') { break; }
                buffer_char = c;
                buffer_size = 1;
                break;
            }
            if (c == '[') {
                current_token[token_pos++] = c;
                return TOKEN_StartOfSequence;
            }
            if (c == ',') {
                current_token[token_pos++] = c;
                return TOKEN_Delimiter;
            }
            if (c == ']') {
                current_token[token_pos++] = c;
                return TOKEN_EndOfSequence;
            }
            if (c == '"') {
                state = STATE_ParseQuotedString;
                break;
            }
            if (('0' <= c && c <= '9') || (c == '-')) {
                state = STATE_ParseNumber;
                current_token[token_pos++] = c;
                break;
            }
            state = STATE_ParseString;
            current_token[token_pos++] = c;
            break;

        case STATE_ParseQuotedString:
            if (c == EOF)
                parse_error("End of file inside quoted string");
            if (c == '"') {
                current_token[token_pos++] = '\0';
                state = STATE_FindStartOfToken;
                return TOKEN_String;
            }
            if (c == '\\') {
                state = STATE_Escape;
                break;
            }
            if (c == '\n') lineno++;
            current_token[token_pos++] = c;
            break;

        case STATE_Escape:
            if (!((c == '"') || (c == '\\'))) {
                parse_error("Illegal character following '\\'");
            }

            current_token[token_pos++] = c;
            state = STATE_ParseQuotedString;
            break;

        case STATE_ParseNumber:
            if (strchr(" \t\n,]#", c) || c == EOF) {
                current_token[token_pos++] = '\0';
                buffer_char = c;
                buffer_size = 1;
                state = STATE_FindStartOfToken;
                return TOKEN_Number;
            }

            if (!('0' <= c && c <= '9')) {
                parse_error("Illegal character following number");
            }
            
            current_token[token_pos++] = c;
            break;
            
        case STATE_ParseString:
            if (strchr(" \t\n,]#", c) || c == EOF) {
                current_token[token_pos++] = '\0';
                buffer_char = c;
                buffer_size = 1;
                state = STATE_FindStartOfToken;
                return TOKEN_String;
            }

            current_token[token_pos++] = c;
            break;
    
        case STATE_EOF:
            break;

        }        

    }

}
