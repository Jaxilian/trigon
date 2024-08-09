#ifndef TR_SHADER_H
#define TR_SHADER_H

#include "trigon/core/std/std.h"


typedef enum {
	SHADER_STAGE_DEFAULT,
	SHADER_STAGE_VERTEX,
	SHADER_STAGE_FRAGMENT
} SHADER_STAGE;

typedef enum {
	GPU_RESOURCE_STATIC,
	GPU_RESOURCE_DYNAMIC,
	GPU_RESOURCE_SAMPLER
} GPU_RESOURCE;


typedef struct {
	cstr_t name;

} shader_info_t;


void shader_new(shader_info_t* info);

void __sys_shader_init();


#endif // !SHADER_H
