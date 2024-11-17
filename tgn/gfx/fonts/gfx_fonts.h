#ifndef GFX_FONTS_H
#define GFX_FONTS_H

#include "cmn/cmn.h"
#ifdef GFX_FONTS_EXPOSE
#endif

void gfx_fonts_load(str_t font_path, float font_size);
void gfx_shader_load_defaults();

#endif // !GFX_FONTS_H
