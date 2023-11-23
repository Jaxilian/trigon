#ifndef T_STD_H
#define T_STD_H
#include "defines.h"

void debug_log(cstr format, ...);
void debug_dump(cstr file_path, cstr format, ...);
void validate(bool expression, cstr format, ...);
void split_string(cstr* str, cstr* delimiter, char** buffer, uint32_t buffer_len);
#endif // !T_STD_H
