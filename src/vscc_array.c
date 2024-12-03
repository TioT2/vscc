/**
 * @brief array utility implementation file
 */

#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include "vscc.h"

/// @brief array internal representation
typedef struct __VsccArrayImpl {
    union {
        struct {
            size_t elementSize; ///< size of single array element
            size_t size;        ///< current array size
            size_t capacity;    ///< current array capacity
        };
        max_align_t _aligner; ///< array alignment forcer
    };

    uint8_t data[1];    ///< data array
} VsccArrayImpl;

/**
 * @brief array for new capacity reallocation function
 * 
 * @param[in] array array to reallocate ptr (non-null)
 * 
 * @return true if reallocated successfully, false otherwise
 */
bool vsccArrayRealloc( VsccArray *array, size_t newCapacity ) {
    assert(array != NULL);
    VsccArray impl = *array;
    assert(impl != NULL);

    size_t oldCapacity = impl->capacity;

    VsccArray newImpl = (VsccArray)realloc(impl, sizeof(VsccArray) + newCapacity * impl->elementSize);
    if (newImpl == NULL)
        return false;

    newImpl->capacity = newCapacity;

    // fill uninitialized bytes with zeros
    if (newCapacity >= oldCapacity)
        memset(
            newImpl->data + newImpl->elementSize * oldCapacity,
            0,
            (newCapacity - oldCapacity) * newImpl->elementSize
        );

    *array = newImpl;

    return true;
} // vsccArrayRealloc

VsccArray vsccArrayCtor( size_t elementSize ) {
    assert(elementSize > 0);

    VsccArray array = (VsccArray)calloc(1, sizeof(VsccArrayImpl));

    if (array == NULL)
        return NULL;

    array->elementSize = elementSize;

    return array;
} // vsccArrayCtor

void vsccArrayDtor( VsccArray array ) {
    free(array);
} // vsccArrayDtor

size_t vsccArraySize( const VsccArray array ) {
    assert(array != NULL);
    return array->size;
} // vscCArraySize

void * vsccArrayData( VsccArray array ) {
    assert(array != NULL);
    return array->data;
} // vsccArrayData

bool vsccArrayPush( VsccArray *array, const void *data ) {
    assert(array != NULL);
    assert(data != NULL);

    VsccArray impl = *array;
    assert(impl != NULL);

    if (impl->size >= impl->capacity) {
        size_t newCapacity = impl->capacity == 0
            ? 4
            : impl->capacity * 2;
        if (!vsccArrayRealloc(array, newCapacity))
            return false;
        impl = *array;
    }

    memcpy(
        (uint8_t *)impl->data + impl->elementSize * impl->size,
        data,
        impl->elementSize
    );

    impl->size++;

    return true;
} // vsccArrayPush

bool vsccArrayPop( VsccArray *array, void *data ) {
    assert(array != NULL);

    VsccArray impl = *array;
    assert(impl != NULL);

    if (impl->size == 0)
        return false;

    if (data != NULL)
        memcpy(
            data,
            (uint8_t *)impl->data + impl->elementSize * (impl->size - 1),
            impl->elementSize
        );

    impl->size--;

    return true;
} //  // vsccArrayPop

// vscc_array.c
