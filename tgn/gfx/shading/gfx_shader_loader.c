#include "gfx/shading/shaders/headers/fonts_default.h"
#include "gfx_shading.h"

gfx_shader_t font_shader = { 0 };

void gfx_shader_load_defaults() {

    gfx_shader_info_t info = { 0 };
    info.vert_bin = (uint32_t*)fonts_default_vert;
    info.vert_bin_len = fonts_default_vert_len;
    info.frag_bin = (uint32_t*)fonts_default_frag;
    info.frag_bin_len = fonts_default_frag_len;

    gfx_shader_new(&info, &font_shader);

   

}

void gfx_shader_unload_defaults() {
    gfx_shader_del(&font_shader);
}