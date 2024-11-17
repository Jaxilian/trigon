#include "gfx_fonts.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	float x0, y0, x1, y1; // Texture coordinates
	float xOff, yOff;     // Offset when rendering
	float xAdvance;       // Advance to next character
} glyph_info_t;

#define FIRST_CHAR 32
#define CHAR_COUNT 95
#define ATLAS_WIDTH 512
#define ATLAS_HEIGHT 512

void gfx_fonts_load(str_t font_path, float font_size) {

	unsigned char* buffer;
	size_t buffer_size;

	FILE* file = fopen(font_path, "rb");
	if (!file) {
		debug_err("failed to open font file %s\n", font_path);
		return;
	}

	fseek(file, 0, SEEK_END);
	buffer_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = malloc(buffer_size);
	if (!buffer) {
		debug_err("failed to allocate memory for font %s\n", font_path);
		return;
	}

	fread(buffer, 1, buffer_size, file);
	fclose(file);

	stbtt_fontinfo font_info;
	if (!stbtt_InitFont(&font_info, buffer, stbtt_GetFontOffsetForIndex(buffer, 0))) {
		debug_err("failed to initialize font %s\n", font_path);
		return;
	}

	float scale = stbtt_ScaleForPixelHeight(&font_info, font_size);

	unsigned char* atlas_bitmap = calloc(ATLAS_WIDTH * ATLAS_HEIGHT, sizeof(unsigned char));
	if (!atlas_bitmap) {
		debug_err("failed to allocate memory for font atlas %s\n", font_path);
		return;
	}

	stbtt_packedchar char_data[CHAR_COUNT];

	stbtt_pack_context pack_ctx;
	stbtt_PackBegin(&pack_ctx, atlas_bitmap, ATLAS_WIDTH, ATLAS_HEIGHT, 0, 1, NULL);
	stbtt_PackSetOversampling(&pack_ctx, 2, 2); // Optional: Improve quality
	stbtt_PackFontRange(&pack_ctx, buffer, 0, font_size, FIRST_CHAR, CHAR_COUNT, char_data);
	stbtt_PackEnd(&pack_ctx);

	glyph_info_t glyphs[CHAR_COUNT];
	for (int i = 0; i < CHAR_COUNT; i++) {
		stbtt_packedchar* pc = &char_data[i];
		glyph_info_t* gi = &glyphs[i];
		gi->x0 = pc->x0 / (float)ATLAS_WIDTH;
		gi->y0 = pc->y0 / (float)ATLAS_HEIGHT;
		gi->x1 = pc->x1 / (float)ATLAS_WIDTH;
		gi->y1 = pc->y1 / (float)ATLAS_HEIGHT;
		gi->xOff = pc->xoff;
		gi->yOff = pc->yoff;
		gi->xAdvance = pc->xadvance;
	}


	free(buffer);
}