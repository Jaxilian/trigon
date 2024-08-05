#include "std.h"
#include <stdio.h>
void version_to_string(char out[64], const version_t* version) {
	snprintf(
		out,
		64,
		"%u.%u.%u.%u",
		version->version,
		version->major,
		version->minor,
		version->patch
	);
}