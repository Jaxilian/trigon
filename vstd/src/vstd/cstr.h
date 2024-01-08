#ifndef CSTR_H
#define CSTR_H
#include <string.h>
#include <stdbool.h>

#define cstr const char*

bool cstr_to_wchar(cstr str, wchar_t* wchar);
bool wchar_to_cstr(const wchar_t* wchar, char* str);
bool string_split(cstr in, cstr delimiter, char** out, int out_len);

#endif