/* ---- SUPER FASTLY WRITTEN MEMORY MANAGER | Rewrite! ---- */

#include "cstd.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#ifdef _DEBUG
static cpool_t pool = { 0 };
#else
static uint32_t allocations = 0;
#endif


cmem_t* __cmem_malloc(str4_t tag, size_t size, cstr_t file, uint32_t line) {
#ifdef _DEBUG
    if (pool.data == NULL) {
        cpool_new(&pool, sizeof(cmem_t), 16);
    }

    size_t tagsize = strlen(tag);
    size_t reqsize = sizeof(str4_t);
        
    cassert(
        tagsize < reqsize,
        "\"tag\" is longer than 4 chars %s:%d\n",
        file, line
    );

    cmem_t mem = {
        .file = file,
        .line = line,
        .ptr = malloc(size),
        .size = size
    };

    cassert(mem.ptr, "failed to alloc memory! %s:%d\n", file, line);
    strcpy(mem.tag, tag);

    uint32_t id = cpool_add(&pool, &mem);
    cmem_t* ptr = (cmem_t*)cpool_get(&pool, id);
    ptr->id = id;

    return ptr;
#else
    allocations++;
    return malloc(size);
#endif
}

void cmem_realloc(cmem_t* ptr, size_t new_size) {
#ifdef _DEBUG
    vptr_t vptr = realloc(ptr->ptr, new_size);
    cassert(
        vptr,
        "failed to reallocate memory! tried to resize pointer from = %s:%d\n",
        ptr->file,
        ptr->line
    );

    ptr->ptr = vptr;
    ptr->size = new_size;
#else
    vptr_t vptr = realloc(ptr, new_size);
    cassert(
        vptr,
        "failed to reallocate memory!\n"
    );
    *ptr = vptr;
#endif
}

void cmem_free(cmem_t* ptr) {
#ifdef _DEBUG
    if (ptr == NULL || ptr->ptr == NULL) {
        return;
    }

    free(ptr->ptr);
    ptr->ptr = NULL; 

    cpool_pop(&pool, ptr->id);

    if (pool.length <= 0) {
        cpool_del(&pool);
        memset(&pool, 0, sizeof(cpool_t));
    }
#else
    allocations--;
    free(ptr);
#endif
}


uint32_t cmem_alloc_count() {
#ifdef _DEBUG
    return pool.length;
#else
    return allocations;
#endif
}


#ifdef _DEBUG

void cmem_print() {
    size_t total_size = 0;

    for (uint32_t i = 0; i < pool.max_count; i++) {
        cmem_t* mem = cpool_get(&pool, i);
        if (mem->ptr == NULL) continue;

        total_size += mem->size;
    }

    printf("memory size: %zu\n", total_size);
}


void cmem_check() {
    for (uint32_t i = 0; i < pool.max_count; i++) {
        cmem_t* ptr = (cmem_t*)cpool_get(&pool, i);
        if (ptr->ptr == NULL) continue;
        
        printf(
            "----------------------------------\n"
            "Found memory!\n"
            "tag: %s\n"
            "file: %s\n"
            "line: %d\n"
            "size: %zu\n"
            "----------------------------------\n",
            ptr->tag,ptr->file, ptr->line, ptr->size
        );
    }
}
#endif