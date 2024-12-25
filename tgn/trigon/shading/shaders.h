#ifndef TRIGON_SHADER_H
#define TRIGON_SHADER_H
#include "gfx/gfx.h"
#include <inttypes.h>

typedef enum {
	SHADER_STAGE_FRAGMENT,
	SHADER_STAGE_VERTEX
} SHADER_STAGE;

typedef enum {
	SHADER_BUFFER_TYPE_STATIC,
	SHADER_BUFFER_TYPE_DYNAMIC,
	SHADER_BUFFER_TYPE_SAMPLER
} SHADER_BUFFER_TYPE;

typedef struct {
	const char*		pack;
	const char*		name;
	win_t*			window;
} shader_info_t;

typedef struct {
	int placeholder;
} shader_t;

void shader_new(shader_t* shader, shader_info_t* info);
void shader_del(shader_t* shader);

#endif // !SHADER_H
