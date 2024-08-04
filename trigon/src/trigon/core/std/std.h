#pragma once


/* ------------------------------ INCLUDES ----------------------------------*/


#include <inttypes.h>
#include <stdarg.h>
#include <string.h>


/* ------------------------------- DEFINES ----------------------------------*/


typedef void*       vptr_t;
typedef void        (*vptrcb_t)();
typedef int32_t     i32;
typedef int64_t     i64;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef float       f32;
typedef double      f64;
typedef const char* cstr_t;
typedef bool        b8;

/* ------------------------------- VERSION ----------------------------------*/


class version_t {
public:
    version_t() {};
    version_t(u32 version, u32 major, u32 minor, u32 patch);
    void set(u32 version, u32 major, u32 minor, u32 patch);

    u32     _version    = 1;
    u32     _major      = 0;
    u32     _minor      = 0;
    u32     _patch      = 0;
    char    _text[16]   = {}; //Max 999.999.999.999

    u32 combined() const {
        return  ((((u32)(_version)) << 29U) | \
                (((u32)(_major)) << 22U) | \
                (((u32)(_minor)) << 12U) | \
                ((u32)(_patch)));
    }

    cstr_t str() const;
    cstr_t str();

private:
};


/* ------------------------------- STRINGS ----------------------------------*/


class str_t {
public:
     str_t(cstr_t txt = nullptr, ...);
    ~str_t();

    void        log();
    cstr_t      cstr();
    u32         length() const { return buff_len; }

    static void wchar(char* in, wchar_t* out);

private:
    char*   buff        = NULL;
    u32     buff_len    = 0;
};


/* ----------------------------- DEBUG_LOG ----------------------------------*/


enum DEBUG_TYPE {
    DEBUG_LOG,
    DEBUG_WRN,
    DEBUG_ERR
};

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

#define cassert(expr, ...) do { \
    if (!(expr)) { \
        _debug_logger_add(\
        true,\
        DEBUG_ERR,\
        __FILE__,\
        __LINE__,\
        __VA_ARGS__\
        ); \
    } \
} while (0)


void debug_log_clear();


/* ----------------------------- DEBUG_LOG ----------------------------------*/

