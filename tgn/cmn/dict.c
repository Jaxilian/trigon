#include "dict.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

static unsigned long hash_string(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void dict_new(dict_t* dict, size_t initial_capacity) {
    assert(dict != NULL && "dict ptr was NULL");

    size_t capacity = 1;
    while (capacity < initial_capacity) {
        capacity <<= 1;
    }

    dict->table_size = capacity;
    dict->table = calloc(dict->table_size, sizeof(dict_pair_t*));
    assert(dict->table != NULL && "Failed to allocate memory for hash table");

    dict->pairs = malloc(capacity * sizeof(dict_pair_t));
    assert(dict->pairs != NULL && "Failed to allocate memory for dict pairs");

    dict->capacity = capacity;
    dict->count = 0;
    dict->free_list = NULL;

    // Initialize the free list
    for (size_t i = 0; i < capacity; ++i) {
        dict->pairs[i].key = NULL; // Mark as unused
        dict->pairs[i].next = dict->free_list;
        dict->free_list = &dict->pairs[i];
    }
}

static void* value_alloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Failed to allocate memory for value\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

bool dict_add(dict_t* dict, const char* key, void* value, size_t value_size) {
    unsigned long hash = hash_string(key);
    size_t index = hash % dict->table_size;

    dict_pair_t* pair = dict->table[index];
    while (pair) {
        if (strcmp(pair->key, key) == 0) {
            // Update existing value
            free(pair->value);
            pair->value = value_alloc(value_size);
            memcpy(pair->value, value, value_size);
            pair->value_size = value_size;
            return false;
        }
        pair = pair->next;
    }

    if (!dict->free_list) {
        // Expand the pool
        size_t new_capacity = dict->capacity * 2;
        dict_pair_t* new_pairs = realloc(dict->pairs, new_capacity * sizeof(dict_pair_t));
        if (!new_pairs) {
            fprintf(stderr, "Failed to expand dict pairs pool\n");
            exit(EXIT_FAILURE);
        }
        dict->pairs = new_pairs;

        // Initialize new entries and add them to the free list
        for (size_t i = dict->capacity; i < new_capacity; ++i) {
            dict->pairs[i].key = NULL;
            dict->pairs[i].next = dict->free_list;
            dict->free_list = &dict->pairs[i];
        }

        dict->capacity = new_capacity;
    }

    // Pop a pair from the free list
    dict_pair_t* new_pair = dict->free_list;
    dict->free_list = dict->free_list->next;

    // Initialize the new pair
    new_pair->key = strdup(key);
    if (!new_pair->key) {
        fprintf(stderr, "Failed to allocate memory for key\n");
        exit(EXIT_FAILURE);
    }
    new_pair->value = value_alloc(value_size);
    memcpy(new_pair->value, value, value_size);
    new_pair->value_size = value_size;

    // Insert into the hash table
    new_pair->next = dict->table[index];
    dict->table[index] = new_pair;
    dict->count++;

    return true;
}

void* dict_get(dict_t* dict, const char* key) {
    unsigned long hash = hash_string(key);
    size_t index = hash % dict->table_size;

    dict_pair_t* pair = dict->table[index];
    while (pair) {
        if (strcmp(pair->key, key) == 0) {
            return pair->value;
        }
        pair = pair->next;
    }

    return NULL;
}

bool dict_pop(dict_t* dict, const char* key) {
    unsigned long hash = hash_string(key);
    size_t index = hash % dict->table_size;

    dict_pair_t* pair = dict->table[index];
    dict_pair_t* prev = NULL;

    while (pair) {
        if (strcmp(pair->key, key) == 0) {
            // Remove from the hash table
            if (prev) {
                prev->next = pair->next;
            }
            else {
                dict->table[index] = pair->next;
            }

            // Free key and value
            free(pair->key);
            free(pair->value);
            pair->key = NULL; // Mark as unused

            // Add the pair back to the free list
            pair->next = dict->free_list;
            dict->free_list = pair;
            dict->count--;

            return true;
        }
        prev = pair;
        pair = pair->next;
    }

    return false;
}

void dict_loop(dict_t* dict, void (*foreach)(const char*, void*)) {
    for (size_t i = 0; i < dict->table_size; ++i) {
        dict_pair_t* pair = dict->table[i];
        while (pair) {
            foreach(pair->key, pair->value);
            pair = pair->next;
        }
    }
}

void dict_del(dict_t* dict) {
    if (!dict) return;

    // Free keys and values
    for (size_t i = 0; i < dict->capacity; ++i) {
        if (dict->pairs[i].key) {
            free(dict->pairs[i].key);
            free(dict->pairs[i].value);
        }
    }

    // Free the hash table
    free(dict->table);
    dict->table = NULL;

    // Free the pairs pool
    free(dict->pairs);
    dict->pairs = NULL;

    dict->capacity = dict->count = 0;
    dict->free_list = NULL;

    memset(dict, 0, sizeof(dict_t));
}

void dict_shrink(dict_t* dict) {
    size_t new_capacity = dict->capacity;
    while (new_capacity / 2 >= dict->count && new_capacity > 16) {
        new_capacity /= 2;
    }

    if (new_capacity < dict->capacity) {
        dict_pair_t* new_pairs = malloc(new_capacity * sizeof(dict_pair_t));
        if (!new_pairs) {
            fprintf(stderr, "Failed to shrink dict pairs pool\n");
            exit(EXIT_FAILURE);
        }

        // Copy active pairs
        size_t index = 0;
        for (size_t i = 0; i < dict->capacity; ++i) {
            if (dict->pairs[i].key != NULL) {
                new_pairs[index++] = dict->pairs[i];
            }
        }

        // Update free list
        dict->free_list = NULL;
        for (size_t i = index; i < new_capacity; ++i) {
            new_pairs[i].key = NULL;
            new_pairs[i].next = dict->free_list;
            dict->free_list = &new_pairs[i];
        }

        free(dict->pairs);
        dict->pairs = new_pairs;
        dict->capacity = new_capacity;
    }
}
