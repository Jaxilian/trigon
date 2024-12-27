#ifndef TRIGON_SHADER_H
#define TRIGON_SHADER_H
#include "gfx/gfx.h"
#include <inttypes.h>
#include "gfx/pipe/gfx_pipe.h"

typedef struct {
	const char* pack;
	const char* name;
	gfx_pipe_t	pipe;
	win_t*		win;
} shader_t;

void shader_new(shader_t* shader, shader_info_t* info);
void shader_del(shader_t* shader);
void shaders_clear();

/*
void tgn_pipe_layout_gen(
	shader_res_set_t sets[MAX_DESC_SETS_IN_USE],
	uint32_t sets_count,
	VkPipelineLayout* out
);
*/

#endif // !SHADER_H
