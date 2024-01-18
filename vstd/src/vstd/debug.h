#ifndef VSTD_DEBUG_H
#define VSTD_DEBUG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    LOG_COL_GREEN,
    LOG_COL_RED,
    LOG_COL_YELLOW,
    LOG_COL_CYAN,
    LOG_COL_DEFAULT
} LOG_COL;

typedef enum {
    BYTE_BYTE,
    BYTE_KB,
    BYTE_MB,
    BYTE_GB,
    BYTE_TB
} byte_type_e;

void convert_bytes(size_t size, byte_type_e* out_type, double* out_size);

void debug_log(const char* format, ...);
void debug_log_col(LOG_COL color, const char* format, ...);
void debug_dump(const char* file_path, const char* format, ...);

void validate(bool expression, const char* format, ...);

#endif // !VSTD_DEBUG_H

