#include "util.h"
#include <stdio.h>

void version_to_str(const version_t* v, char str[64]) {
	snprintf(str, sizeof(str), "%u.%u.%u.%u", v->version, v->major, v->minor, v->patch);
}

