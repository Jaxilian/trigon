#ifndef TG_DEBUG_H
#define TG_DEBUG_H

#include <stdarg.h>
#include "util.h"

#if defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#elif defined(__GNUC__) || defined(__clang__)
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

typedef const char* cstr_t;

NORETURN void debug_err(cstr_t format, ...);
void debug_wrn(cstr_t format, ...);
void debug_log(cstr_t format, ...);

#endif // !TG_DEBUG_H