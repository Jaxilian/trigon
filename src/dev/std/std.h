#ifndef T_STD_H
#define T_STD_H
#include "defines.h"

typedef void (*signal_t)();

typedef enum {
	LOG_COL_GREEN,
	LOG_COL_RED,
	LOG_COL_YELLOW,
	LOG_COL_CYAN,
	LOG_COL_DEFAULT
} LOG_COL;

void debug_log(cstr format, ...);
void debug_log_col(LOG_COL color, cstr format, ...);
void debug_dump(cstr file_path, cstr format, ...);

void validate(bool expression, cstr format, ...);

void split_string(cstr* str, cstr* delimiter, char** buffer, uint32_t buffer_len);


#endif // !T_STD_H
