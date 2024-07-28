#ifndef __CSTD_H
#define __CSTD_H

#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>


/* ------------------------------- DEFINES ----------------------------------*/


#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

typedef char        tag_t[4];
typedef const char* cstr_t;
typedef void*		vptr_t;

typedef struct {
    uint32_t version;
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
} version_t;


/* -------------------------------- CASSERT ---------------------------------*/


void _assert_dump(const char* file, int line, const char* format, ...);
void cassert_enable_file_dump(bool val);

#define cassert(expr, ...) do { \
    if (!(expr)) { \
        _assert_dump(__FILE__, __LINE__, __VA_ARGS__); \
    } \
} while (0)


/* -------------------------------- STRINGS ---------------------------------*/

typedef char str4_t[4];
typedef char str64_t[64];

void cstr_to_wchar(cstr_t str, wchar_t* wchar);
void wchar_to_cstr(const wchar_t* wchar, char* str);
void version_to_str(const version_t* v, str64_t str);
uint32_t cstr_split(cstr_t in, cstr_t delimiter, char** out);

#define version_to_int(v) ((((uint32_t)((v).version)) << 29U) | \
                           (((uint32_t)((v).major)) << 22U) | \
                           (((uint32_t)((v).minor)) << 12U) | \
                           ((uint32_t)((v).patch)))


/* -------------------------------- FLAGS -----------------------------------*/


#define flag_new(num)               1 << num
#define flag_set(flags, flag)       flags |= flag
#define flag_off(flags, flag)       flags &= ~flag;
#define flag_active(flags, flag)    flags & flag
#define flag_toggle(flags, flag)    flags ^= FLAG_B;
#define flag_shiftl(flags, num)     flags << num
#define flag_shiftr(flags, num)     flags >> num
typedef uint32_t flags_t;


/* -------------------------------  CPOOL  ----------------------------------*/


typedef struct {
    uint32_t    max_count;  // max slots
    uint32_t    length;     // occupied slots
    size_t      stride;     // size of each slots
    void* data;       // buffer

    uint32_t    queue_out_ptr;  // next id going out
    uint32_t    queue_in_ptr;   // next id going in
    uint32_t* queue;          // queue buffer
} cpool_t;

inline 
vptr_t      cpool_get(cpool_t* pool, uint32_t id);
void        cpool_new(cpool_t* pool, size_t stride, uint32_t preset_length);
uint32_t    cpool_add(cpool_t* pool, vptr_t data);
uint32_t    cpool_len(cpool_t* pool);
void        cpool_pop(cpool_t* pool, uint32_t index);
void        cpool_del(cpool_t* pool);
size_t      cpool_mem(cpool_t* pool);


/* ------------------------------  MEMORY  ----------------------------------*/

#ifdef _DEBUG
typedef struct {
    uint32_t    id;
    str4_t      tag;
    vptr_t      ptr;
    cstr_t      file;
    uint32_t    line;
    size_t      size;
} cmem_t;

cmem_t*     __cmem_malloc(str4_t tag, size_t size, cstr_t file, uint32_t line);
void        cmem_realloc(cmem_t* ptr, size_t new_size);
void        cmem_free(cmem_t* ptr);
void        cmem_print();
uint32_t    cmem_alloc_count();
void        cmem_check();

#define cmem_malloc(tag, size) __cmem_malloc(tag, size, __FILE__,__LINE__)
#define cmem_get(p)p->ptr

#else
typedef void* cmem_t;
cmem_t*     __cmem_malloc(str4_t tag, size_t size, cstr_t file, uint32_t line);
void        cmem_realloc(cmem_t* ptr, size_t new_size);
vptr_t      cmem_ptr();
void        cmem_free(cmem_t* ptr);
#define     cmem_malloc(tag, size) __cmem_malloc(tag, size, __FILE__,__LINE__)
#define     cmem_print()
uint32_t    cmem_alloc_count();
#define     cmem_check()
#define     cmem_get(p)p
#endif


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


typedef uint8_t md5_t[16];
typedef char md5str_t[33]; // + 1 for the null terminator

void md5_str(char* input, md5_t result);
void md5_file(FILE* file, md5_t result);
void md5_to_str(md5_t hash, md5str_t output);


#endif // !__CSTD_H
