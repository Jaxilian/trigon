#ifndef CLIB_DICTONARY_H
#define CLIB_DICTONARY_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct dict_pair_t {
    char* key;                 // Dynamically allocated key string
    void* value;               // Pointer to the value data
    size_t value_size;         // Size of the value data
    struct dict_pair_t* next;  // Next entry in the collision chain
} dict_pair_t;

typedef struct {
    dict_pair_t** table;       // Hash table array
    size_t table_size;         // Number of buckets in the hash table
    dict_pair_t* pairs;        // Array of dict_pair_t entries (memory pool)
    size_t capacity;           // Capacity of the pairs array
    size_t count;              // Number of used pairs
    dict_pair_t* free_list;    // Linked list of free dict_pair_t entries
} dict_t;

void dict_new(size_t initial_capacity, dict_t* dict);
void dict_del(dict_t* dict);
bool dict_add(dict_t* dict, const char* key, void* value, size_t value_size);
bool dict_pop(dict_t* dict, const char* key);
void* dict_get(dict_t* dict, const char* key);
void dict_loop(dict_t* dict, void (*foreach)(const char*, void*));
void dict_shrink(dict_t* dict);

#endif // !CLIB_DICTONARY_H
