#include <stdlib.h>
#include <stdio.h>

void OOM_GUARD(void *ptr, char* file, int number){

    if (ptr == NULL) {
        printf("Out of memory Error. Error Occured at %s, %d", file, number);
        exit(EXIT_FAILURE);
    }
}