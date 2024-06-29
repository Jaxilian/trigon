#ifndef TG_VFS_H
#define TG_VFS_H

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#include <inttypes.h>
#include <stdbool.h>

typedef enum {
	VFS_TYPE_UNKOWN,
	VFS_TYPE_FILE,
	VFS_TYPE_DIR
} VFS_TYPE;

typedef char vpath_t[MAX_PATH];


bool vfs_exist(const vpath_t path);
bool vfs_isdir(const vpath_t path);
bool vfs_isfile(const vpath_t path);
bool vfs_create(const vpath_t parent, const char* name, VFS_TYPE type);
bool vfs_init(const char* path, vpath_t out);

#endif