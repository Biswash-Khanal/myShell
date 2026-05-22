#ifndef VECTOR_H
#define VECTOR_H

/**
 * @brief Structure defining the vector implementation. Basically a dynamic array capable of storing any datatype.
 *
 */
typedef struct Vector
{
    //size of individual elements
    const size_t itemSize;

    //number of items currently
    size_t length;

    //number of items it can currently store in its current state before realloc
    size_t capacity;

    //the pointer to the start of the vector
    void* buffer;

} Vector;

/**
 * @brief Create a Vector object
 *
 * @param itemsize The size of individual item
 * @param capacity Initial capacity
 * @return Vector
 */
Vector vec_createVector(size_t itemsize, size_t capacity);

/**
 * @brief Deletes the Vector object
 *
 * @param vector The vector to be deleted
 */
void vec_deleteVector(Vector* self);

//getter functions

/**
 * @brief Get the Vector Length object
 *
 * @param self
 * @return size_t
 */
size_t vec_getVectorLength(const Vector* self);



/**
 * @brief Get the Buffer Address object
 *
 * @param self
 * @return void*
 */
const void* vec_getBufferAddress(const Vector* self);

//Operations

/**
 * @brief Returns the address of the buffer memory cell of the index
 *
 * @param self
 * @param index
 * @return void*
 */
void* vec_getItemAddress(const Vector* self, size_t index);


/**
 * @brief Copies the value of an index to a location outAddress
 *
 * @param self
 * @param index
 * @param outAddress
 */
void vec_getItem(const Vector* self, size_t index, void* outAddress);

/**
 * @brief Copies the value from the location inAddress to the index of buffer. When the index value is equal to the length, the value is appended to the buffer.
 *
 * @param self
 * @param index
 * @param inAddress
 */
void vec_setItem(const Vector* self, size_t index, void* inAddress);


#endif