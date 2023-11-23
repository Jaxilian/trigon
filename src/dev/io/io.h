#ifndef T_IO_H
#define T_IO_H
#include "dev/std/std.h"

typedef struct {
	unsigned char*	data;
	size_t			size;
	uint32_t		width;
	uint32_t		height;
	uint32_t		channels;
} file_img_t;

void file_image_open(file_img_t* img);
void file_image_close(file_img_t* img);
void file_image_write();
void file_build_map_grid(cstr_t file_path, cstr_t name, cstr_t out_path);

#endif // !T_IO_H
