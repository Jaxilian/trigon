#ifndef TG_UTILS_H
#define TG_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdbool.h>
#include "debug.h"

typedef void (*voidcb_t)();

typedef struct  {
	uint32_t version;
	uint32_t major;
	uint32_t minor;
	uint32_t patch;
} version_t;


bool cstr_to_wchar(const char* str, wchar_t* wchar);
bool wchar_to_cstr(const wchar_t* wchar, char* str);
bool string_split(const char* in, const char* delimiter, char** out, int out_len);
void version_to_str(const version_t* v, char str[64]);

#define version_to_int(v) ((((uint32_t)((v).version)) << 29U) | \
                           (((uint32_t)((v).major)) << 22U) | \
                           (((uint32_t)((v).minor)) << 12U) | \
                           ((uint32_t)((v).patch)))


#ifdef __cplusplus
}
#endif

#endif // !TG_UTILS_H