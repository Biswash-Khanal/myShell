#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "Vector.h"
#include "MemoryGuard.h"


/**
 * @brief Utility function to expand the current capacity of a vector in different situations appropriately.
 *
 * @param self The vector whose capacity is to be expanded.
 * @param n The incoming capacity need.
 */
static void expandCapacity(Vector* self, size_t n) {

    /**
     * @brief Logic used:
     * We check if the incoming request is greater or equal to current capacity,
     *      if not, we simply return
     *
     *      if yes,
     *              we realloc the buffer to a size two times the incoming request.
     *              assign the new address from realloc to the current buffer of the vector
     *              then update the capacity value of the vector
     *              return
     */

    if (n >= self->capacity) {
        self->buffer = realloc(self->buffer, n * 2 * self->itemSize);
        MEMORY_GUARD(self->buffer, __FILE__, __LINE__);
        self->capacity = n * 2;
    }
    return;
}

Vector vec_createVector(size_t itemsize, size_t capacity) {
    //create and initialize a new vector according to the argument itemsize and capacity.
    Vector newVector = {
        .itemSize = itemsize,
        .length = 0,
        .capacity = capacity,
        .buffer = calloc(capacity, itemsize) };
    MEMORY_GUARD(newVector.buffer, __FILE__, __LINE__);
    return newVector;
}

void vec_deleteVector(Vector* self) {
    //we free the buffer first, then nullify the static values
    free(self->buffer);
    self->buffer = NULL;
    self->length = 0;
    self->capacity = 0;
}

size_t vec_getVectorLength(const Vector* self) {
    //simply return the length value.
    return self->length;
}

const void* vec_getBufferAddress(const Vector* self) {
    //simply return the buffer address.
    return self->buffer;
}

void* vec_getItemAddress(const Vector* self, size_t index) {
    //multiply item size with the given index to get the offset, then add it to the buffer address by first casting to a byte size datatype pointer(char). 
    //then cast it to void pointer and return. 
    return (void*)((const char*)(self->buffer) + (index * self->itemSize));
}

void vec_getItem(const Vector* self, size_t index, void* outAddress) {
    //directly use memcpy to copy the data from vector index of size itemSize to the outaddress.
    memcpy(outAddress, vec_getItemAddress(self, index), self->itemSize);
}

void vec_setItem(Vector* self, size_t index, void* inAddress) {
    //run the utility to check if enough space available by giving it the incoming index.
    expandCapacity(self, index);

    //now that we are sure theres enough space allocated, simply run the memcpy function.
    memcpy(vec_getItemAddress(self, index), inAddress, self->itemSize);

    //update the length in case an index greater than current length was passed
    if (index + 1 >= self->length) {
        self->length = index + 1;
    }
}

void vec_pushBack(Vector* self, void* inAddress) {
    //simply use the setItem function at index = current length
    vec_setItem(self, self->length, inAddress);

}