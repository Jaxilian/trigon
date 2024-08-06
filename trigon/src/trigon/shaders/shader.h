#ifndef TR_SHADER_H
#define TR_SHADER_H

#include "trigon/core/std/std.h"

typedef struct {
	cstr_t name;

} shader_info_t;


void shader_new(shader_info_t* info);

void __sys_shader_init();


#endif // !SHADER_H
