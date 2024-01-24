#ifndef VKL_FONTS_H
#define VKL_FONTS_H
#include "vkl.h"

typedef void* vkl_font_t;

vkl_font_t vkl_fonts_new(const char* font_file);
void vkl_fonts_get(vkl_font_t face, char ch, unsigned int font_width, unsigned int font_height);
void vkl_fonts_del(vkl_font_t face);
#endif // !VKL_FONTS_H
