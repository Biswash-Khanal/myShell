#ifndef VECTOR_H
#define VECTOR_H


#include <stdlib.h>


typedef struct Vector
{

    const size_t itemSize;


    size_t length;


    size_t capacity;


    void* buffer;

} Vector;


Vector vec_createVector(size_t itemsize, size_t capacity);


void vec_deleteVector(Vector* self);




size_t vec_getVectorLength(const Vector* self);




const void* vec_getBufferAddress(const Vector* self);




void* vec_getItemAddress(const Vector* self, size_t index);



void vec_getItem(const Vector* self, size_t index, void* outAddress);


void vec_setItem(Vector* self, size_t index, void* inAddress);



void vec_pushBack(Vector* self, void* inAddress);

#endif