#ifndef MEMORY_GUARD_H
#define MEMORY_GUARD_H



/**
 * @brief Function to check the results of a malloc, calloc or a realloc function to make sure it succeeded
 *
 * @param memoryPointer The value returned by the memory function
 * @return void
 */
void MEMORY_GUARD(void* memoryPointer, char* filename, int fileline);


#endif