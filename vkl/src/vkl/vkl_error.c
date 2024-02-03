#include "vkl.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdarg.h>
static void validate(bool expression, const char* format, ...) {
#ifdef _DEBUG

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


_Noreturn void vkl_error(const char* msg, vkl_error_enum type) {
#ifdef _DEBUG
	printf("%s\n", msg);
#else
    validate(false, msg);
#endif
	if (type == ERROR_FATAL) {
		exit(1);
	}
}