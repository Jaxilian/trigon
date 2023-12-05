#ifndef TRIGON_T
#define TRIGON_T
#include "dev/std/std.h"
#include <cglm/cglm.h>

typedef struct {
	uint32_t		width;
	uint32_t		height;
	uint32_t		channels;
	unsigned char*	data;
} texture_t;

void texture_sample(texture_t* tex, vec2 position, vec4 color);

#endif // !TRIGON_T
