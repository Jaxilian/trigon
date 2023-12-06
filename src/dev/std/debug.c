#include "std.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#ifdef _WIN32
static void win_set_color(LOG_COL color) {
    switch (color) {
        case LOG_COL_GREEN:
            printf("\033[92m");
            return;
        case LOG_COL_RED:
            printf("\033[91m");
            return;
        case LOG_COL_YELLOW:
            printf("\033[33m");
            return;
        case LOG_COL_CYAN:
            printf("\033[96m");
            return;
        case LOG_COL_DEFAULT:
            printf("\033[0m");
            return;
        default:
            printf("\033[0m");
            return;
    }
}
#define set_color win_set_color
#else
#define set_color
#endif
#endif

void debug_log_col(LOG_COL color, cstr format, ...) {
#ifdef _DEBUG
    set_color(color);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    set_color(LOG_COL_DEFAULT);
#endif
}

void debug_dump(cstr file_path, cstr format, ...) {
    FILE* file = fopen(file_path, "a");
    if (file != NULL) {
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fprintf(file, "\n");
        fclose(file);
    }
}

void debug_log(cstr format, ...) {
#ifdef _DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
}

void validate(bool expression, cstr format, ...) {
#ifdef _DEBUG
    if (!expression) {
        debug_log("[");
        debug_log_col(LOG_COL_RED, "ERROR");
        debug_log("] ");

        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
        exit(1);
    }
#else
    if (!expression) {
        FILE* file = fopen("error_log.txt", "a");
        if (file != NULL) {
            va_list args;
            va_start(args, format);
            vfprintf(file, format, args);
            va_end(args);
            fprintf(file, "\n");
            fclose(file);
        }
        exit(1);
    }
#endif
}

void convert_bytes(size_t size, byte_type_e* out_type, double* out_size){
    if (size > 1000000000) {
        *out_type = BYTE_GB;
        *out_size = (double)size / 1000000000.0f;
    }
    else if (size > 1000000) {
        *out_type = BYTE_MB;
        *out_size = (double)size / 1000000.0f;
    }
    else if (size > 1000) {
        *out_type = BYTE_KB;
        *out_size = (double)size / 1000.0f;
    }
    else {
        *out_type = BYTE_BYTE;
        *out_size = (double)size;
    }
}