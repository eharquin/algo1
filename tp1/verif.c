#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include "verif_func.h"
#include "parse.h"

FILE *fh; /* file handle */

/*
 * Display the number of milliseconds that has elapsed since start.
 */
clock_t timer(clock_t start) {
    clock_t diff = clock()-start;
    int msec = diff*1000/CLOCKS_PER_SEC;
    printf("    time: %d ms\n", msec);
    return diff;
}

void usage() {
    printf("Usage: verif [options] [testfile1] [testfile2] ...\n");
    printf("  -h   Print this message and exit.\n");
    /* printf("  -t   Time each test.\n"); */
    printf("  -v   Verbose.\n");
}

int main(int argc, char* argv[]) {

    int c;
    int vflag = 0; /* , tflag = 0; */

    if (argc == 1) {
        usage();
        return 0;
    }

    while ((c = getopt(argc, argv, "hv")) != -1)
        switch (c) {
            case 'h':
                usage();
                return 0;
            case 'v':
                /* tflag = 1; */
                vflag = 1;
                break;
            case '?':
                usage();
                return 1;
            default:
                abort();
        }


    int tried = 0, passed = 0;

    int i;
    for (i = optind; i < argc; i++) {

        fh = fopen(argv[i], "r");
        if (fh == NULL) {
            fprintf(stderr, "Error opening test file: \"%s\"\n", argv[i]);
            continue;
        }

        if (vflag)
            printf("Opening test file: \"%s\"\n", argv[i]);

        char test_id[MAX_STRING_LENGTH];
        char test_function[MAX_STRING_LENGTH];

        while (read_test_header(test_id, test_function)) {

            if (vflag)
                printf("*** Running test \033[1m%s\033[0m ...\n", test_id);

            int res = -1;

            /*
            clock_t msec;
            if (tflag) msec = clock();
            */

            /* run test case */
            res = run_test(test_id, test_function);
            tried++;

            if (res == 1) {
                if (vflag)
                    printf("Test \033[1m%s \033[92mPASSED\033[0m\n", test_id);
                passed++;
            } else {
                printf("Test \033[1m%s \033[91mFAILED\033[0m\n", test_id);
            }

            /*
            if (tflag) timer(msec);
            */
        }

        fclose(fh);
        
    }

    printf("%d of %d tests passed\n", passed, tried);

    return 0;
}
