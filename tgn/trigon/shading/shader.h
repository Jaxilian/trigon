#ifndef TRIGON_SHADER_H
#define TRIGON_SHADER_H
#include "gfx/gfx.h"
#include <inttypes.h>

typedef struct{
	unsigned char*	data;
	uint32_t		data_len;
} shader_data_t;

typedef struct {
	shader_data_t vertex_data;
	shader_data_t fragment_data;
	win_t* window;
} shader_info_t;

typedef struct {
	int placeholder;
} shader_t;

void shader_new(shader_t* shader, shader_info_t* info);
void shader_del(shader_t* shader);

#endif // !SHADER_H
