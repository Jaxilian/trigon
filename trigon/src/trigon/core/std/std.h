#ifndef TRIGON_STD_H
#define TRIGON_STD_H
#include <stdbool.h>
#include <stdarg.h>

/*--------------------------------- DEFINES ---------------------------------*/

typedef const char*			cstr_t;
typedef void*				vptr_t;
typedef void				(*vptrcb_t)();

typedef bool                b8;

typedef char				u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;

typedef signed char			i8;
typedef signed short		i16;
typedef signed int			i32;
typedef signed long long	i64;

typedef float	            f32;
typedef double	            f64;

/*--------------------------------- VERSION ---------------------------------*/

typedef struct {
    u32 version;
    u32 major;
    u32 minor;
    u32 patch;
} version_t;

void version_to_string(char out[64], const version_t* version);
#define version_to_int(v) ((((uint32_t)((v).version)) << 29U) | \
                           (((uint32_t)((v).major)) << 22U) | \
                           (((uint32_t)((v).minor)) << 12U) | \
                           ((uint32_t)((v).patch)))


/*---------------------------------- MATH -----------------------------------*/

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define NEXT_POWER_OF_TWO(number)if (number <= 2) {number = 2; } \
else {u32 num = 2; while (number > num) { \
num *= 2; } number = num;}


/*---------------------------------- LOGS -----------------------------------*/

typedef enum {
    DEBUG_LOG,
    DEBUG_WRN,
    DEBUG_ERR
} DEBUG_TYPE;

void _debug_logger_add(
    bool force_quit,
    DEBUG_TYPE type,
    cstr_t file,
    int line,
    cstr_t format,
    ...
);

#define debug_log(...) _debug_logger_add( \
    false, \
    DEBUG_LOG, \
    __FILE__, \
    __LINE__, \
    __VA_ARGS__ \
)

#define debug_wrn(...) _debug_logger_add( \
    false, \
    DEBUG_WRN, \
    __FILE__, \
    __LINE__, \
    __VA_ARGS__ \
)

#define debug_err(...) _debug_logger_add( \
    false, \
    DEBUG_ERR, \
    __FILE__, \
    __LINE__, \
    __VA_ARGS__ \
)

#define cassert(expr, ...)  do { \
    if (!(expr)) { \
        _debug_logger_add(\
        true,\
        DEBUG_ERR,\
        __FILE__,\
        __LINE__,\
        __VA_ARGS__\
        ); exit(1); \
    } \
} while (0)


void debug_log_clear();


/* -------------------------------  CPOOL  ----------------------------------*/


typedef struct {
    u32         max_count;  // max slots
    u32         length;     // occupied slots
    size_t      stride;     // size of each slots
    vptr_t      data;       // buffer

    u32         queue_out_ptr;  // next id going out
    u32         queue_in_ptr;   // next id going in
    u32*        queue;          // queue buffer
} cpool_t;

inline
vptr_t      cpool_get(cpool_t* pool, u32 id);
void        cpool_new(cpool_t* pool, size_t stride, u32 preset_length);
u32         cpool_add(cpool_t* pool, vptr_t data);
u32         cpool_len(cpool_t* pool);
void        cpool_pop(cpool_t* pool, u32 index);
void        cpool_del(cpool_t* pool);
size_t      cpool_mem(cpool_t* pool);


/* ------------------------------ FILE SYSTEM ------------------------------- */


#define __TG_FS_MAX 260
typedef char fs_t[__TG_FS_MAX];

void fs_usr(fs_t dest);
void fs_root(fs_t dest);
void fs_append(fs_t dest, fs_t extra);
bool fs_exist(fs_t dest);
bool fs_isfile(fs_t dest);
bool fs_find(fs_t path, fs_t child);
void fs_get(fs_t path, fs_t child);
void fs_valid(fs_t path);
int  fs_ls(fs_t path, fs_t* out, const char* exc_ext, bool recursive);
void fs_ext(fs_t dest, fs_t path);
void fs_name(fs_t dest, fs_t path);
void fs_rel(fs_t dest, fs_t ancestor, fs_t descandant);
void fs_dir(fs_t dest);
void fs_app(fs_t dest);
void fs_parent(fs_t dest, fs_t path);
void fs_cd(fs_t dest, fs_t new_dest);

/* ------------------------------ MD5 HASHING -------------------------------*/


typedef u8 md5_t[16];
typedef char md5str_t[33]; // + 1 for the null terminator

void md5_str(char* input, md5_t result);
void md5_file(vptr_t file, md5_t result);
void md5_to_str(md5_t hash, md5str_t output);

/* ------------------------------ FNV1A HASHING -----------------------------*/

u32 fnv1a_hash(cstr_t text);

u32 murmur3_32(const u8* key, u64 len, u32 seed);
u64 murmur3_64(const u8* key, u64 len, u32 seed);


/* ------------------------------- HASHMAP.c --------------------------------*/

/*
The MIT License (MIT)

Copyright (c) 2020 Joshua J Baker

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Modified by Jax Carls,
changes on the original code is copyrighted by Jax Carls

*/

struct hashmap_t;

struct hashmap_t* hashmap_new(size_t elsize, size_t cap, u64 seed0,
    u64 seed1,
    u64(*hash)(const vptr_t  item, u64 seed0, u64 seed1),
    int (*compare)(const vptr_t  a, const vptr_t  b, vptr_t  udata),
    void (*elfree)(vptr_t  item),
    vptr_t  udata);

struct hashmap_t* hashmap_new_with_allocator(vptr_t(*malloc)(size_t),
    vptr_t(*realloc)(vptr_t, size_t), void (*free)(vptr_t), size_t elsize,
    size_t cap, u64 seed0, u64 seed1,
    u64(*hash)(const vptr_t  item, u64 seed0, u64 seed1),
    int (*compare)(const vptr_t  a, const vptr_t  b, vptr_t  udata),
    void (*elfree)(vptr_t  item),
    vptr_t  udata);

void hashmap_free(struct hashmap_t* map);
void hashmap_clear(struct hashmap_t* map, b8 update_cap);
size_t hashmap_count(struct hashmap_t* map);
b8 hashmap_oom(struct hashmap_t* map);
const vptr_t  hashmap_get(struct hashmap_t* map, const vptr_t  item);
const vptr_t  hashmap_set(struct hashmap_t* map, const vptr_t  item);
const vptr_t  hashmap_delete(struct hashmap_t* map, const vptr_t  item);
const vptr_t  hashmap_probe(struct hashmap_t* map, u64 position);

b8 hashmap_scan(
    struct hashmap_t* map,
    b8(*iter)
    (const vptr_t  item, vptr_t udata),
    vptr_t  udata
);

b8 hashmap_iter(
    struct hashmap_t* map, size_t* i, vptr_t* item);

const vptr_t  hashmap_get_with_hash(
    struct hashmap_t* map, const vptr_t  key, u64 hash);

const vptr_t  hashmap_delete_with_hash(
    struct hashmap_t* map, const vptr_t  key, u64 hash);

const vptr_t  hashmap_set_with_hash(
    struct hashmap_t* map, const vptr_t  item, u64 hash);

void hashmap_set_grow_by_power(struct hashmap_t* map, size_t power);
void hashmap_set_load_factor(struct hashmap_t* map, f64 load_factor);


#endif // !TRIGON_STD_H
