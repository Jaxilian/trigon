#include "debug.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void debug_err(cstr_t format, ...) {
    printf("[");
    printf(ANSI_COLOR_RED "err" ANSI_COLOR_RESET);
    printf("] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}

void debug_wrn(cstr_t format, ...) {
    printf("[");
    printf(ANSI_COLOR_YELLOW "wrn" ANSI_COLOR_RESET);
    printf("] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void debug_log(cstr_t format, ...) {
    printf("[");
    printf(ANSI_COLOR_GREEN "log" ANSI_COLOR_RESET);
    printf("] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}