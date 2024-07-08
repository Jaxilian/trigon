#ifndef TG_TEXTURE_H
#define TG_TEXTURE_H
#include "trigon/core/graphics/vkdef.h"
#include "trigon/core/vfs/vfs.h"
#include <inttypes.h>

typedef unsigned char uc_t;

typedef struct  {
	uint32_t	width;
	uint32_t	height;
	uint32_t	channels;
	uint32_t	pixel_count;
	uc_t*		pixels;
}texture_info_t;

texture_info_t	texture_read(vpath_t path);
uint32_t		texture_new(texture_info_t* info);
void			texture_del();

#endif // !TEXTURE_H
