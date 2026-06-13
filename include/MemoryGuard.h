#ifndef MEMORY_GUARD_H
#define MEMORY_GUARD_H

/**
 * @brief A simple check for the pointer returned by functions by the likes of malloc and realloc.
 * 
 * @param memoryPointer The pointer returned by the function.
 * @param filename The name of the src file (__FILE__).
 * @param fileline The line number of the code (__LINE__).
 */
void MEMORY_GUARD(void* memoryPointer, char* filename, int fileline);

#endif