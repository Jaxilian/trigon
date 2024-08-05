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


#endif // !TRIGON_STD_H
