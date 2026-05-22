#include <stdlib.h>
#include <stdio.h>

#include "MemoryGuard.h"


void MEMORY_GUARD(void* memoryPointer, char* filename, int fileline) {
    if (memoryPointer == NULL) {
        fprintf(stderr, "Ran out of memory in file %s, at line %d", filename, fileline);
        exit(EXIT_FAILURE);
    }

    return;

}
