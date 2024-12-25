#ifndef GFX_PIPE_H
#define GFX_PIPE_H
#include "gfx/gfx.h"
#include <inttypes.h>

void gfx_shader_new(const uint32_t* bin, size_t bin_size, VkShaderModule* mod);
	
#endif // !GFX_PIPE_H
