/** Verify Trusty

    Implementation of fprintf()

 */
#include <stdio.h>

int fprintf(FILE *stream, const char *format, ...){
    // avoid unhandled instruction for fprintf
    // treat as noop
    return 1;
}