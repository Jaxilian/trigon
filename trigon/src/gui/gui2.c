#include "gui/gui2.h"
#include <vkl/vkl_fonts.h>
#include <vkl/vkl.h>
#include <vstd/vfs.h>
#include <core/core.h>

#pragma warning(disable : 4116)
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#include "nuklear.h"

static struct nk_context ctx = { 0 };
static struct nk_font_atlas atlas = { 0 };
static struct nk_font* font = NULL;
static const void* image;
static int atlas_width, atlas_height;
static vkl_image_t font_image = { 0 };

void trigon_gui_draw() {
	enum { EASY, HARD };
	static int op = EASY;
	static float value = 0.6f;
	static int i = 20;

	if (nk_begin(&ctx, "Show", nk_rect(50, 50, 220, 220),
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_CLOSABLE)) {
		/* fixed widget pixel width */
		nk_layout_row_static(&ctx, 30, 80, 1);
		if (nk_button_label(&ctx, "button")) {
			/* event handling */
		}

		/* fixed widget window ratio width */
		nk_layout_row_dynamic(&ctx, 30, 2);
		if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
		if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;

		/* custom widget pixel width */
		nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
		{
			nk_layout_row_push(&ctx, 50);
			nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
			nk_layout_row_push(&ctx, 110);
			nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
		}
		nk_layout_row_end(&ctx);
	}
	nk_end(&ctx);


}

void trigon_gui_init() {

	path_os_t path = { 0 };
	path_get_current(&path);
	path_change_dir(&path, "fonts", &path);
	path_find_file(&path, "Vera.ttf", &path);

	nk_init_default(&ctx, 0);
	nk_font_atlas_init_default(&atlas);
	nk_font_atlas_begin(&atlas);
	
	font = nk_font_atlas_add_from_file(&atlas, path.data, 16, 0);
	image = nk_font_atlas_bake(&atlas, &atlas_width, &atlas_height, NK_FONT_ATLAS_RGBA32);

	vkl_image_info_t info = {
		.device = trigon_core_vkl_device(),
		.extent = (VkExtent3D){atlas_width, atlas_height, 1},
		.format = VK_FORMAT_R8G8B8A8_UNORM,
		.mip_levels = 1,
		.type = VK_IMAGE_TYPE_2D,
		.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.use_sampler = true
	};

	vkl_image_new(&info, &font_image);
	vkl_image_set(trigon_core_vkl_state(), &font_image, image);

	nk_font_atlas_end(&atlas, nk_handle_id(1), NULL);
	nk_style_set_font(&ctx, &font->handle);

	
}
