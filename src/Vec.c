#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Guards.h"
#include "Vec.h"
/**
 *@brief Helper function to expand the capacity of the buffer in case needed to expand. Works by setting the new buffer size equal to the incomming request *2
 *
 * @param self
 * @param n the incoming size
 */
static void _ensure_capacity(Vec* self, size_t n);


Vec Vec_value(size_t capacity, size_t item_size) {
    //create a Vec newVec and initialize it properly
    Vec newVec = {
        item_size,
        0,
        capacity,
        //allocate memory in heap for the buffer
        calloc(capacity, item_size)
    };
    //oom check
    OOM_GUARD(newVec.buffer, __FILE__, __LINE__);

    //return the Vec
    return newVec;
}

void Vec_drop(Vec* self) {
    //free the memory taken by the buffer
    free(self->buffer);

    //zero out the other fields
    //make the pointer null so no accidental usage
    self->buffer = NULL;
    self->capacity = 0;
    self->length = 0;
}


size_t Vec_length(const Vec* self) {
    //simple get length
    return self->length;
}
const void* Vec_items(const Vec* self) {
    //simple get the buffer address
    return self->buffer;
}

void* Vec_ref(const Vec* self, size_t index) {
    //return null if either empty vector, or invalid index
    if (index > self->length - 1 || self->length == 0) {
        return NULL;
    }

    char* indexPointer = (char*)self->buffer + index * self->item_size;

    return (void*)indexPointer;

}
void Vec_get(const Vec* self, size_t index, void* out) {
    if (!self || !out) {
        return; // guard against NULL pointers
    }

    void* indexPointer = Vec_ref(self, index);
    if (!indexPointer) {
        return; // invalid index handled by Vec_ref
    }

    memcpy(out, indexPointer, self->item_size);
}

void Vec_set(Vec* self, size_t index, const void* value) {

    if (!self || !value) {
        return; // guard against NULL pointers
    }

    void* indexPointer = Vec_ref(self, index);
    if (!indexPointer) {
        return; // invalid index handled by Vec_ref
    }

    memcpy(index, value, self->item_size);
}




static void _ensure_capacity(Vec* self, size_t n) {
    if (n > self->capacity) {
        size_t new_capacity = n * 2;
        self->buffer = realloc(self, new_capacity * self->item_size);
        OOM_GUARD(self, __FILE__, __LINE__);
        self->capacity = new_capacity;
    }
}






