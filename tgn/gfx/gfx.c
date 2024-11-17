#include "gfx.h"
#include "fonts/gfx_fonts.h"
#include "shading/gfx_shading.h"

void gfx_load_requirements() {
	gfx_shader_load_defaults();
}

void gfx_unload_requirements() {
	gfx_shader_unload_defaults();
}