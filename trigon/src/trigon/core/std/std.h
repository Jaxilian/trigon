#pragma once

#include <inttypes.h>
#include <stdarg.h>
#include <string.h>

typedef void*       vptr_t;
typedef void        (*vptrcb_t)();
typedef int32_t     i32;
typedef int64_t     i64;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef float       f32;
typedef double      f64;
typedef const char* cstr_t;



class version_t {
public:
    version_t() {};
    version_t(u32 version, u32 major, u32 minor, u32 patch);

    u32     _version    = 1;
    u32     _major      = 0;
    u32     _minor      = 0;
    u32     _patch      = 0;
    char    _text[16]   = {}; //Max 999.999.999.999

    u32 combined() {
        return  ((((u32)(_version)) << 29U) | \
                (((u32)(_major)) << 22U) | \
                (((u32)(_minor)) << 12U) | \
                ((u32)(_patch)));
    }

    cstr_t str() const;
    cstr_t str();

private:
};

class str_t {
public:
     str_t(cstr_t txt = nullptr, ...);
    ~str_t();

    void        log();
    cstr_t      cstr();

private:
    char*   buff        = NULL;
    u32     buff_len    = 0;
};

template <u32 T>
class sstr_t {
public:
    sstr_t(cstr_t base = nullptr) {
        if (!base) return;

        u32 l = (u32)strlen(base);
        if (l >= len_max) {
            return;
        }

        len = l;
        strcpy(buffer, base);
    }

    void append(cstr_t s) {
        u32 l = (u32)strlen(s);
        if (l + len >= len_max) {
            return;
        }

        len += l;
        strcat(buffer, s);
    }

    cstr_t str() {
        return buffer;
    }

private:
    u32  len        = 0;
    u32  len_max    = T;
    char buffer[T]  = {};
};