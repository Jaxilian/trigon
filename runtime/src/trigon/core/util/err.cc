#include "err.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>


void debug_t::err(cstr_t format, ...) {
    printf("[err] ");
    va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);

    va_end(args);

    std::exit(EXIT_FAILURE);
}

void debug_t::wrn(cstr_t format, ...) {
    printf("[wrn] ");
    va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);
}

void debug_t::log(cstr_t format, ...) {
    printf("[log] ");
    va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);
}