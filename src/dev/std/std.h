#ifndef T_STD_H
#define T_STD_H
#include "defines.h"

void debug_log(cstr_t format, ...);
void debug_dump(cstr_t file_path, cstr_t format, ...);
void validate(bool expression, cstr_t format, ...);
#endif // !T_STD_H
