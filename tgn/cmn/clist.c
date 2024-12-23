#include "clist.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_TRIM_SIZE 8
#define DEFAULT_INITIAL_CAPACITY 2

static uint32_t get_next_power_of_two(uint32_t number) {
    if (number == 0) return 1;
    uint32_t n = 1;
    while (n < number) {
        n <<= 1;
    }
    return n;
}

void clist_new(clist_t* list, size_t stride, uint32_t initial_capacity) {
    if (initial_capacity < 0) initial_capacity = 0;

    uint32_t capacity = get_next_power_of_two(
        initial_capacity ? initial_capacity : DEFAULT_INITIAL_CAPACITY
    );

    list->capacity = capacity;
    list->length = 0;
    list->stride = stride;
    list->data = malloc(capacity * stride);
    list->free_stack = malloc(capacity * sizeof(uint32_t));

    if (!list->data || !list->free_stack) {
        free(list->data);
        free(list->free_stack);
        list->data = NULL;
        list->free_stack = NULL;
        list->capacity = 0;
        list->length = 0;
        return;
    }

    // Initialize the free stack
    list->free_stack_top = 0;
    for (uint32_t i = 0; i < capacity; i++) {
        list->free_stack[list->free_stack_top++] = capacity - i - 1;
    }
}

uint32_t clist_add(clist_t* list, const void* data) {
    if (list->free_stack_top == 0) {

        uint32_t new_capacity = list->capacity * 2;
        void* new_data = realloc(list->data, new_capacity * list->stride);
        uint32_t* new_free_stack = realloc(list->free_stack, new_capacity * sizeof(uint32_t));

        if (!new_data || !new_free_stack) {
            return UINT32_MAX; // Indicate failure
        }

        list->data = new_data;
        list->free_stack = new_free_stack;

        // Initialize new free indices
        for (uint32_t i = list->capacity; i < new_capacity; i++) {
            list->free_stack[list->free_stack_top++] = new_capacity - i + list->capacity - 1;
        }

        list->capacity = new_capacity;
    }

    assert(list->free_stack_top > 0);

    uint32_t index = list->free_stack[--list->free_stack_top]; // Pop from free stack

    void* slot_ptr = (char*)list->data + index * list->stride;
    if (data) {
        memcpy(slot_ptr, data, list->stride);
    }
    else {
        memset(slot_ptr, 0, list->stride);
    }

    list->length++;
    return index;
}

void* clist_get(clist_t* list, uint32_t index) {
    if (index >= list->capacity) {
        return NULL;
    }
    return (char*)list->data + index * list->stride;
}

void clist_pop(clist_t* pool, uint32_t index) {
    if (index >= pool->capacity) {
        return;
    }

    for (uint32_t i = 0; i < pool->free_stack_top; i++) {
        if (pool->free_stack[i] == index) {
            return;
        }
    }

    pool->free_stack[pool->free_stack_top++] = index;
    pool->length--;

    void* slot_ptr = (char*)pool->data + index * pool->stride;
    memset(slot_ptr, 0, pool->stride);
}

void clist_del(clist_t* list) {
    if (list->data) {
        free(list->data);
        list->data = NULL;
    }
    if (list->free_stack) {
        free(list->free_stack);
        list->free_stack = NULL;
    }
    list->capacity = 0;
    list->length = 0;
    list->free_stack_top = 0;
    list->stride = 0;
}

uint32_t clist_len(clist_t* list) {
    return list->length;
}

size_t clist_mem(clist_t* list) {
    return list->capacity * list->stride + list->capacity * sizeof(uint32_t);
}

void clist_shrink(clist_t* list) {
    uint32_t delta = list->capacity - list->length;

    if (delta < MIN_TRIM_SIZE || list->capacity <= DEFAULT_INITIAL_CAPACITY) {
        return;
    }

    // Calculate new capacity
    uint32_t new_capacity = get_next_power_of_two(list->length);
    if (new_capacity < DEFAULT_INITIAL_CAPACITY) {
        new_capacity = DEFAULT_INITIAL_CAPACITY;
    }

    // Reallocate data array
    void* new_data = realloc(list->data, new_capacity * list->stride);
    if (!new_data) {
        // Failed to reallocate, cannot shrink
        return;
    }
    list->data = new_data;

    // Reallocate free stack
    uint32_t* new_free_stack = realloc(list->free_stack, new_capacity * sizeof(uint32_t));
    if (!new_free_stack) {
        // Failed to reallocate, cannot shrink
        return;
    }
    list->free_stack = new_free_stack;

    // Remove free indices beyond the new capacity
    uint32_t new_free_stack_top = 0;
    for (uint32_t i = 0; i < list->free_stack_top; i++) {
        if (list->free_stack[i] < new_capacity) {
            list->free_stack[new_free_stack_top++] = list->free_stack[i];
        }
    }
    list->free_stack_top = new_free_stack_top;

    list->capacity = new_capacity;
}
