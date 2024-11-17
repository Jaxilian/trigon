#include "cpool.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static uint32_t get_next_power_of_two(uint32_t number) {
    if (number == 0) return 1;
    uint32_t n = 1;
    while (n < number) {
        n <<= 1;
    }
    return n;
}

void cpool_new(cpool_t* pool, size_t stride, uint32_t initial_capacity) {
    uint32_t capacity = get_next_power_of_two(initial_capacity ? initial_capacity : 16);

    pool->capacity = capacity;
    pool->length = 0;
    pool->stride = stride;
    pool->data = malloc(capacity * stride);
    pool->free_stack = malloc(capacity * sizeof(uint32_t));

    if (!pool->data || !pool->free_stack) {
        free(pool->data);
        free(pool->free_stack);
        pool->data = NULL;
        pool->free_stack = NULL;
        pool->capacity = 0;
        pool->length = 0;
        return;
    }

    // Initialize the free stack
    pool->free_stack_top = 0;
    for (uint32_t i = 0; i < capacity; i++) {
        pool->free_stack[pool->free_stack_top++] = capacity - i - 1;
    }
}

uint32_t cpool_add(cpool_t* pool, const void* data) {
    if (pool->free_stack_top == 0) {

        uint32_t new_capacity = pool->capacity * 2;
        void* new_data = realloc(pool->data, new_capacity * pool->stride);
        uint32_t* new_free_stack = realloc(pool->free_stack, new_capacity * sizeof(uint32_t));

        if (!new_data || !new_free_stack) {
            return UINT32_MAX; // Indicate failure
        }

        pool->data = new_data;
        pool->free_stack = new_free_stack;

        for (uint32_t i = pool->capacity; i < new_capacity; i++) {
            pool->free_stack[pool->free_stack_top++] = new_capacity - i + pool->capacity - 1;
        }

        pool->capacity = new_capacity;
    }

    assert(pool->free_stack_top > 0);

    uint32_t index = pool->free_stack[--pool->free_stack_top]; // Pop from free stack

    void* slot_ptr = (char*)pool->data + index * pool->stride;
    if (data) {
        memcpy(slot_ptr, data, pool->stride);
    }
    else {
        memset(slot_ptr, 0, pool->stride);
    }

    pool->length++;
    return index;
}

void* cpool_get(cpool_t* pool, uint32_t index) {
    if (index >= pool->capacity) {
        return NULL;
    }
    return (char*)pool->data + index * pool->stride;
}

void cpool_pop(cpool_t* pool, uint32_t index) {
    if (index >= pool->capacity) {
        // Invalid index
        return;
    }

    pool->free_stack[pool->free_stack_top++] = index; 
    pool->length--;

    void* slot_ptr = (char*)pool->data + index * pool->stride;
    memset(slot_ptr, 0, pool->stride);
}

void cpool_del(cpool_t* pool) {
    if (pool->data) {
        free(pool->data);
        pool->data = NULL;
    }
    if (pool->free_stack) {
        free(pool->free_stack);
        pool->free_stack = NULL;
    }
    pool->capacity = 0;
    pool->length = 0;
    pool->free_stack_top = 0;
    pool->stride = 0;
}

uint32_t cpool_len(cpool_t* pool) {
    return pool->length;
}

size_t cpool_mem(cpool_t* pool) {
    return pool->capacity * pool->stride + pool->capacity * sizeof(uint32_t);
}