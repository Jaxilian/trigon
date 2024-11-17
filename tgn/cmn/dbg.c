#include "cmn.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void assert(bool statement, str_t msg, ...) {
    if (!statement) {
        printf("[err] ");
        va_list args;
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
        exit(EXIT_FAILURE);
    }
}

void debug_err(str_t format, ...) {
    printf("[err] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

void debug_wrn(str_t format, ...) {
    printf("[wrn] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void debug_log(str_t format, ...) {
    printf("[log] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

