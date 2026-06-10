#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "Vector.h"
#include "MemoryGuard.h"



static void expandCapacity(Vector* self, size_t n) {

    if (n >= self->capacity) {
        self->buffer = realloc(self->buffer, n * 2 * self->itemSize);
        MEMORY_GUARD(self->buffer, __FILE__, __LINE__);
        self->capacity = n * 2;
    }
    return;

}

Vector vec_createVector(size_t itemsize, size_t capacity) {
    Vector newVector = {
        .itemSize = itemsize,
        .length = 0,
        .capacity = capacity,
        .buffer = calloc(capacity, itemsize) };
    MEMORY_GUARD(newVector.buffer, __FILE__, __LINE__);
    return newVector;
}

void vec_deleteVector(Vector* self) {

    free(self->buffer);
    self->buffer = NULL;
    self->length = 0;
    self->capacity = 0;
}

size_t vec_getVectorLength(const Vector* self) {
    return self->length;
}

const void* vec_getBufferAddress(const Vector* self) {
    return self->buffer;
}

void* vec_getItemAddress(const Vector* self, size_t index) {
    return (void*)((const char*)(self->buffer) + (index * self->itemSize));
}

void vec_getItem(const Vector* self, size_t index, void* outAddress) {
    memcpy(outAddress, vec_getItemAddress(self, index), self->itemSize);
}


void vec_setItem(Vector* self, size_t index, void* inAddress) {

    expandCapacity(self, index);

    memcpy(vec_getItemAddress(self, index), inAddress, self->itemSize);
    self->length = index + 1;

}

void vec_pushBack(Vector* self, void* inAddress) {
    vec_setItem(self, self->length, inAddress);

}
