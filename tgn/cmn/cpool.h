#ifndef TGN_CPOOL_H
#define TGN_CPOOL_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t capacity;       // Total capacity of the pool
    uint32_t length;         // Number of occupied slots
    size_t   stride;         // Size of each element
    void* data;           // Data array

    uint32_t free_stack_top; // Top index of the free index stack
    uint32_t* free_stack;    // Stack of free indices
} cpool_t;

// Initializes the pool with the given stride and initial capacity.
void cpool_new(cpool_t* pool, size_t stride, uint32_t initial_capacity);

// Adds a new element to the pool, returns its index.
uint32_t cpool_add(cpool_t* pool, const void* data);

// Retrieves a pointer to the element at the given index.
void* cpool_get(cpool_t* pool, uint32_t index);

// Removes an element from the pool by index.
void cpool_pop(cpool_t* pool, uint32_t index);

// Deletes the pool and frees all resources.
void cpool_del(cpool_t* pool);

// Returns the number of occupied slots in the pool.
uint32_t cpool_len(cpool_t* pool);

// Returns the total memory used by the pool.
size_t cpool_mem(cpool_t* pool);

#endif // !TGN_CPOOL_H
