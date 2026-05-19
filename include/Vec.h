#ifndef VEC_H
#define VEC_H

#include <stdlib.h>

/**
 *@brief // Vec structure for a dynamic array of any type
 * @var
 */
typedef struct Vec
{
    // Size of the single item in bytes
    const size_t item_size;

    // Length of items in the vector
    size_t length;

    //Maximum-Length of items the vector can hold at its current state
    size_t capacity;

    //A void pointer to the start of the vector heap memory
    void* buffer;
} Vec;

/*  These are the member functions capable of accessing the
 Vec structure. Ideally, only usage of the Vec structure
 Should be made with the help of these functions only
*/


/**
 *@brief Function to contstuct the Vec value
 *
 * @param capacity initial number of items the vec can store
 * @param item_size sizeof of an individial item
 * @return Vec returns the initialized Vec Value
 */
Vec Vec_value(size_t capacity, size_t item_size);


/**
 *@brief deletes and frees the vector
 *
 * @param self the vector pointer itself
 */
void Vec_drop(Vec* self);

// getters
/**
 *@brief  returns the length of the vector
 *
 * @param self the vector itself
 * @return size_t the length of the vector
 */
size_t Vec_length(const Vec* self);


/**
 *@brief gives access to read all of the vector elements without giving modification access
 *
 * @param self the vector itself
 * @return const void* an immutable pointer to the vector
 */
const void* Vec_items(const Vec* self);

// Operations

/**
 *@brief get a reference back to a specific index of the vector
 *
 * @param self pointer to the vector itself
 * @param index the index of which address is required
 * @return void*
 */
void* Vec_ref(const Vec* self, size_t index);

/**
 *@brief Copies the value of at index to the address out
 *
 * @param self
 * @param index the index of which the value is to be transferred
 * @param out the address where the value is to be transferred
 */
void Vec_get(const Vec* self, size_t index, void* out);

/**
 *@brief Copies the value from the value's address to the index
 *
 * @param self
 * @param index the index at which value is to be stored
 * @param value the address of value to be stored
 */
void Vec_set(Vec* self, size_t index, const void* value);



#endif