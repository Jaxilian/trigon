#ifndef TGN_WIN_H
#define TGN_WIN_H

#include <inttypes.h>
#include "core/gfxctx.h"

typedef const char* str_t;

typedef struct {
	uint32_t version;
	uint32_t major;
	uint32_t minor;
	uint32_t patch;
} version_t;

#define VERSION_TO_INT(v) ((((uint32_t)((v).version)) << 29U) | \
                           (((uint32_t)((v).major)) << 22U) | \
                           (((uint32_t)((v).minor)) << 12U) | \
                           ((uint32_t)((v).patch)))

void gfx_init(str_t app_name, version_t app_version);
void gfx_quit();

void win_new(win_t* win, str_t win_name, uint32_t width, uint32_t height, uint32_t flags);
void win_frame_begin(win_t* win);
void win_frame_end(win_t* win);
void win_del(win_t* win);

#if _USING_VULKAN_SDK
void gfx_load_requirements();
void gfx_unload_requirements();
#endif

#endif // !TGN_WIN_H
