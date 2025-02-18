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

void __debug_err(const char* file, int line, str_t format, ...) {
    printf("[err] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    printf("(%s : %d)\n ", file, line);

    //exit(EXIT_FAILURE);
}

void __debug_wrn(const char* file, int line, str_t format, ...) {
    printf("[wrn] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    printf("(%s : %d)\n ", file, line);
}

void __debug_log(const char* file, int line, str_t format, ...) {
    printf("[log] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    printf("(%s : %d)\n ", file, line);
}

