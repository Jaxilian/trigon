#include "gfx/shading/shaders/headers/fonts_default.h"
#include "gfx_shading.h"

static gfx_shader_t font_shader = { 0 };

void gfx_shader_load_defaults() {



}

void gfx_shader_unload_defaults() {
    gfx_shader_del(&font_shader);
}