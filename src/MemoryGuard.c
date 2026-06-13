#include <stdlib.h>
#include <stdio.h>


#include "MemoryGuard.h"


void MEMORY_GUARD(void* memoryPointer, char* filename, int fileline) {
    //If the returned memory pointer was null, there was an error and we exit. Else, we return and continue normally.
    if (memoryPointer == NULL) {
        fprintf(stderr, "Ran out of memory in file %s, at line %d", filename, fileline);
        exit(EXIT_FAILURE);
    }
    return;
}
