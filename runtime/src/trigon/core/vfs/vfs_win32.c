#ifdef _WIN32
#include "vfs.h"
#include <Windows.h>
#include "trigon/core/utils/debug.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define WIN_PATH_END_STD '\\'
#define UNIX_PATH_END_STD '/'

static void convert_winstd(vpath_t path) {
	uint32_t len = strlen(path);
	for (uint32_t i = 0; i < len; i++) {
		if (path[i] == UNIX_PATH_END_STD) {
			path[i] = WIN_PATH_END_STD;
		}
	}
}

static void validate_path(vpath_t path) {
	convert_winstd(path);

	uint32_t parent_length = strlen(path);
	char lastchar = path[parent_length - 1];
	if (lastchar != WIN_PATH_END_STD && lastchar != UNIX_PATH_END_STD) {
		strcat(path, WIN_PATH_END_STD);
	}
}

bool vfs_exist(const vpath_t path) {
	DWORD dwAttrib = GetFileAttributesA(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES);
}

bool vfs_isdir(const vpath_t path) {
	DWORD attributes = GetFileAttributesA(path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool vfs_isfile(const vpath_t path) {
	DWORD attributes = GetFileAttributesA(path);
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return false;
	}
	return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool vfs_create(const vpath_t parent, const char* name, VFS_TYPE v) {
	
	vpath_t new_path = { 0 };
	strcpy(new_path, parent);

	validate_path(new_path);
	strcat(new_path, name);

	if (v == VFS_TYPE_DIR) {

		if (vfs_exist(new_path)) {
			return false;
		}

		char cmd[MAX_PATH + 9];
		snprintf(cmd, sizeof(cmd), "mkdir %s", new_path);
		return system(cmd) == 0;
	}
	else if (v == VFS_TYPE_FILE) {
		FILE* file = fopen(new_path, "w");
		if (file == NULL) {
			debug_err("failed to create file: %s\n", new_path);
			return false;
		}

		fclose(file);
	}

	return true;
}

bool vfs_init(const char* path, vpath_t out) {
	if (!vfs_exist(path)) {
		memset(out, 0, sizeof(out));
		debug_wrn("%s does not exist\n", path);
		return false;
	}

	strcpy(out, path);
	return true;
}


#endif