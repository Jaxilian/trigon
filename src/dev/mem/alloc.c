#include "mem.h"
#include <stdlib.h>

#ifdef _DEBUG
#include <stdio.h>
#include <stdint.h>
#include <memory.h>

typedef struct {
    bool        initialized;
    ptr_t       pointer;
    cstr_t      file;
    uint32_t    line;
    size_t      size;
} mem_ptr_t;

static mem_ptr_t pointers[512] = { 0 };
static uint32_t allocations = 0;

static size_t fnv1a_hash_x32(ptr_t ptr, size_t max) {
    uintptr_t val = (uintptr_t)ptr;
    const size_t fnv_prime = 0x811C9DC5; 
    size_t hash = 0x811C9DC5;

    for (size_t i = 0; i < sizeof(val); ++i) {
        hash ^= (val & 0xff);
        hash *= fnv_prime;
        val >>= 8;
    }

    return hash % max;
}

static size_t fnv1a_hash_x64(ptr_t ptr, size_t max) {
    uintptr_t val = (uintptr_t)ptr;
    const size_t fnv_prime = 0x100000001B3;
    size_t hash = 0xCBF29CE484222325; 

    for (size_t i = 0; i < sizeof(val); ++i) {
        hash ^= (val & 0xff);
        hash *= fnv_prime;
        val >>= 8;
    }

    return hash % max;
}


ptr_t cmem_alloc(size_t size, cstr_t file, uint32_t line) {
    validate(allocations < 512, "max allocations reached [512]! change max stack in alloc.c\n");
 
    ptr_t ptr = malloc(size);
    validate(ptr == NULL,
        "failed to allocate memory!\n"
        "information:\n"
        "    size-> %zu\n"
        "    file-> %s\n"
        "    line-> %d",
        size, file, line
    );

    uint32_t ptr_id = (uint32_t)fnv1a_hash_x64(ptr, 512);
    validate(!pointers[ptr_id].initialized,"pointer hash collision!\n");
    pointers[ptr_id].initialized = true;
    pointers->file = file;
    pointers->line = line;
    pointers->pointer = ptr;
    pointers->size = size;
    allocations++;


    return ptr;
}


void  cmem_free(ptr_t ptr) {
    if (ptr == NULL) return;
    uint32_t ptr_id = (uint32_t)fnv1a_hash_x64(ptr, 512);

    if (!pointers[ptr_id].initialized) {
        printf("failed to find pointer in cache! [alloc.c]\n");
        free(ptr);
        return;
    }

    free(ptr);
    memset(&pointers[ptr_id], 0, sizeof(mem_ptr_t));
    allocations--;
}

void  cmem_check() {
    for (uint32_t i = 0; i < 512; i++) {
        if (pointers[i].initialized) {
            printf(
                "failed to allocate memory!\n"
                "information:\n"
                "    size-> %zu\n"
                "    file-> %s\n"
                "    line-> %d",
                pointers[i].size,
                pointers[i].file,
                pointers[i].line
            );
        }
    }
}
#endif