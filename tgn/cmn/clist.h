#ifndef CLIST_CIRCULAR_LIST_H
#define CLIST_CIRCULAR_LIST_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t    capacity;       // Total capacity of the pool
    uint32_t    length;         // Number of occupied slots
    size_t      stride;         // Size of each element
    void*       data;           // Data array

    uint32_t    free_stack_top; // Top index of the free index stack
    uint32_t*   free_stack;    // Stack of free indices
} clist_t;

// Initializes the pool with the given stride and initial capacity.
void clist_new(clist_t* pool, size_t stride, uint32_t initial_capacity);

// Adds a new element to the pool, returns its index.
uint32_t clist_add(clist_t* pool, const void* data);

// Retrieves a pointer to the element at the given index.
void* clist_get(clist_t* pool, uint32_t index);

// Removes an element from the pool by index.
void clist_pop(clist_t* pool, uint32_t index);

// Deletes the pool and frees all resources.
void clist_del(clist_t* pool);

// Returns the number of occupied slots in the pool.
uint32_t clist_len(clist_t* pool);

// Returns the total memory used by the pool.
size_t clist_mem(clist_t* pool);

// Removes all unsed slots and decreases memory being used at a cost
void clist_shrink(clist_t* pool);

#endif // !CLIST_CIRCULAR_LIST_H
