#include "cstd.h"

#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static bool enable_file_dump = false;

void cassert_enable_file_dump(bool val) {
    enable_file_dump = val;
}

void _assert_dump(cstr_t file, int line, cstr_t format, ...) {
    va_list args;
    va_start(args, format);

    char message[1024];
    vsnprintf(message, sizeof(message), format, args);

    if (enable_file_dump) {
        FILE* log_file = fopen("ERROR", "a");
        if (log_file != NULL) {
            fprintf(log_file, "ERR [%s:%d] > %s\n", file, line, message);
            fclose(log_file);
        }
        else {
            fprintf(stderr, "Could not open error log file\n");
        }
    }

    cstr_t syserr = strerror(errno);
    if (!syserr) syserr = "none";
    if (strcmp(syserr, "Success") == 0) syserr = "None";
    fprintf(stderr, "ERR [%s:%d] %s > %s\n", file, line, syserr, message);

    va_end(args);
    exit(-1);
}
