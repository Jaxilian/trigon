#include "defines.h"

#include <string.h>
#include <stdio.h>

cstr_t version_t::string() {
	sprintf_s(str, sizeof(str), "%u.%u.%u.%u\0", version, major, minor, patch);
	return str;
}

