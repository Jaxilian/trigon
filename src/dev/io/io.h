#ifndef T_IO_H
#define T_IO_H
#include "dev/std/std.h"

#define MAX_FILE_CHILDREN 32

typedef struct {
	cstr		name;
	void*		children[MAX_FILE_CHILDREN];
	void*		parent;
	bool		is_directory;
} file_t;

typedef struct {
	unsigned char*	data;
	size_t			size;
	uint32_t		width;
	uint32_t		height;
	uint32_t		channels;
} file_img_t;

void	file_close();
char*	file_open(cstr path, size_t* out_size);

void file_img_open(cstr path, file_img_t* image);
void file_img_close(file_img_t* img);
void file_img_write(cstr path, file_img_t* image);
void file_img_split_to_grid(cstr file_path, cstr name, cstr out_path);

void path_new_root(cstr path);
cstr path_get_root();
cstr path_get_path(cstr path);

//returns path provided + file name, this is a singleton so a new call will replace the old one!
cstr path_get_file(cstr path, cstr name); 
#endif // !T_IO_H
