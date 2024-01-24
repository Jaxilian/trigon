#include "vkl_fonts.h"

#include <ft2build.h>
#include FT_FREETYPE_H

static bool freetype_initialized = false;
static unsigned int active_fonts;
static FT_Library ft;

inline static void check_init() {
	if (freetype_initialized) return;

	if (FT_Init_FreeType(&ft)) {
		vkl_error("could not init FreeType Library\n", ERROR_FATAL);
		return;
	}

	freetype_initialized = true;
}

vkl_font_t vkl_fonts_new(const char* font_file) {
	check_init();

	FT_Face face;
	if (FT_New_Face(ft, font_file, 0, &face)) {
		printf("font file: %s\n", font_file);
		vkl_error("failed to load font\n", ERROR_FATAL);
		return NULL;
	}
	active_fonts++;
	return face;
}

void vkl_fonts_del(vkl_font_t face) {
	if (FT_Done_Face(face)) {
		vkl_error("failed to delete font!\n", ERROR_WARNING);
		return;
	}

	active_fonts--;
	if (active_fonts <= 0) {
		FT_Done_FreeType(ft);

	}
}

void vkl_fonts_get(vkl_font_t face, char ch, unsigned int font_width, unsigned int font_height) {
	FT_Set_Pixel_Sizes(
		face,  
		font_width,    
		font_height);  

	if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
		vkl_error("failed to load Character\n", ERROR_WARNING);
		return;
	}


}