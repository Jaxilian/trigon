#include "std.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void validate(bool expression, cstr format, ...) {
#ifdef _DEBUG
    if (!expression) {
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