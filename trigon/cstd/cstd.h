#ifndef TG_CSTD_H
#define TG_CSTD_H

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

typedef char		tag_t[4];
typedef const char* cstr_t;

typedef struct {
	uint32_t version;
	uint32_t major;
	uint32_t minor;
	uint32_t patch;
} version_t;


/* -------------------------------- STRINGS ---------------------------------*/


bool cstr_to_wchar(const char* str, wchar_t* wchar);
bool wchar_to_cstr(const wchar_t* wchar, char* str);
bool string_split(const char* in, const char* delimiter, char** out, int out_len);
void version_to_str(const version_t* v, char str[64]);

#define version_to_int(v) ((((uint32_t)((v).version)) << 29U) | \
                           (((uint32_t)((v).major)) << 22U) | \
                           (((uint32_t)((v).minor)) << 12U) | \
                           ((uint32_t)((v).patch)))


/* ------------------------------ FILE SYSTEM -------------------------------*/


#define __TG_FS_MAX 260
typedef char fs_t[__TG_FS_MAX];

void fs_usr(fs_t dest);
void fs_root(fs_t dest);
void fs_append(fs_t dest, fs_t extra);
bool fs_exists(fs_t dest);
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
void fs_cp(fs_t dest, fs_t src);


/* -------------------------------- FLAGS -----------------------------------*/


#define flag_new(num)               1 << num
#define flag_set(flags, flag)       flags |= flag
#define flag_off(flags, flag)       flags &= ~flag;
#define flag_active(flags, flag)    flags & flag
#define flag_toggle(flags, flag)    flags ^= FLAG_B;
#define flag_shiftl(flags, num)     flags << num
#define flag_shiftr(flags, num)     flags >> num
typedef uint32_t flags_t;


void _assert_dump(const char* file, int line, const char* format, ...);
void assert_enable_file_dump(bool val);

#define cassert(expr, ...) do { \
    if (!(expr)) { \
        _assert_dump(__FILE__, __LINE__, __VA_ARGS__); \
    } \
} while (0)

/* -------------------------------  CPOOL  ----------------------------------*/


typedef struct {
	uint32_t	max_count;	// max slots
	uint32_t	length;		// occupied slots
	size_t		stride;		// size of each slots
	void*		data;		// buffer

	uint32_t	queue_out_ptr;	// next id going out
	uint32_t	queue_in_ptr;	// next id going in
	uint32_t*	queue;			// queue buffer
} cpool_t;

void		cpool_new(cpool_t* pool, size_t stride, uint32_t preset_length);
uint32_t	cpool_add(cpool_t* pool, void* data);
uint32_t	cpool_len(cpool_t* pool);
#define		cpool_get(pool, id) (void*)((char*)pool->data+id*pool->stride);
void		cpool_pop(cpool_t* pool, uint32_t index);
void		cpool_del(cpool_t* pool);
size_t		cpool_mem(cpool_t* pool);


/* ------------------------------ MD5 HASHING -------------------------------*/


typedef uint8_t md5_t[16];
typedef char md5str_t[33]; // + 1 for the null terminator

void md5_str(char* input, md5_t result);
void md5_file(FILE* file, md5_t result);
void md5_to_str(md5_t hash, md5str_t output);



/* ------------------------------  MEMORY  ----------------------------------*/


#ifdef MEMORY_DEBUG
//void _cstd_mem_alloc(size_t size, const char* file, uint32_t line);

#define mem_alloc(size) malloc(size)
#define mem_free(ptr) free(ptr)
#else

#endif
#endif // !
