#ifndef TG_UTILS_H
#define TG_UTILS_H
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

void version_to_str(const version_t* v, char str[64]);

#define version_to_int(v) ((((uint32_t)((v).version)) << 29U) | \
                           (((uint32_t)((v).major)) << 22U) | \
                           (((uint32_t)((v).minor)) << 12U) | \
                           ((uint32_t)((v).patch)))

#endif // !TG_UTILS_H
