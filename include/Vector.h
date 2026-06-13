#ifndef VECTOR_H
#define VECTOR_H


#include <stdlib.h>

/**
 * @brief The vector data structure.
 * 
 */
typedef struct Vector
{
    const size_t itemSize;  //The size of an individual element to be stored in the vector. Const so that it cant be changed after initialization.
    size_t length;          //Carries the current number of elements stored in the vector.
    size_t capacity;        //The max number of elements the vector can hold at its current state. 
    void* buffer;           //The dynamically allocated storage space for storing the actual data.
} Vector;

/**
 * @brief Creates a new vector for specified itemsize and given capacity.
 * 
 * @param itemsize The size of an individual item to be stored in the vector.
 * @param capacity The initial max number of elements to be allowed to be stored without expanding.
 * @return Vector Returns the created vector.
 */
Vector vec_createVector(size_t itemsize, size_t capacity);

/**
 * @brief Frees up the dynamically allocated memory and nulls static memory for all the elements of the given vector.
 * 
 * @param self The vector to be freed.
 */
void vec_deleteVector(Vector* self);



/**
 * @brief Getter for the length of the vector.
 * 
 * @param self The vector whose length is to be returned.
 * @return size_t The length value of the vector.
 */
size_t vec_getVectorLength(const Vector* self);



/**
 * @brief Getter for the buffer address of the vector.
 * 
 * @param self The vector whose buffer address is to be returned.
 * @return const void* The pointer to the start of the buffer address of the vector.
 */
const void* vec_getBufferAddress(const Vector* self);



/**
 * @brief Getter for the buffer address of a specific indexed element of the vector. 
 * 
 * @param self The vector whose buffer address is to be returned.
 * @param index The index at which, the buffer address of it is to be returned.
 * @return void* The pointer to the buffer address at the given index.
 */
void* vec_getItemAddress(const Vector* self, size_t index);


/**
 * @brief Retrieves an element from the vector and makes a copy of it at the given address.
 * 
 * @param self The vector whose element is to be retrieved.
 * @param index The index at which, the element to be retrieved is located.
 * @param outAddress The address where the element is to be stored at.
 */
void vec_getItem(const Vector* self, size_t index, void* outAddress);


/**
 * @brief Retrieved an item from the given address and stores it at the given index of the vector.
 * 
 * @param self The vector where the item is to be inserted.
 * @param index The index at which the item is to be inserted.
 * @param inAddress The address from where the item is to be inserted.
 */
void vec_setItem(Vector* self, size_t index, void* inAddress);


/**
 * @brief Simplified function to simply insert a new item at the end of the vector.
 * 
 * @param self The vector where the item is to be inserted.
 * @param inAddress The address from where the item is to be inserted.
 */
void vec_pushBack(Vector* self, void* inAddress);

#endif