#pragma once
#ifndef VELIGHT_VFS_H
#define VELIGHT_VFS_H

#include <inttypes.h>
#include <stdbool.h>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#define WIN_PATH_END_STD '\\'
#define UNIX_PATH_END_STD '/'

#ifdef _WIN32
#define STD_PATH_END WIN_PATH_END_STD
#else
#define STD_PATH_END UNIX_PATH_END_STD
#endif

typedef enum {
	VFS_TYPE_UNKOWN,
	VFS_TYPE_FILE,
	VFS_TYPE_DIR
} VFS_TYPE;


typedef char vpath_t[MAX_PATH];

bool cstr_to_wchar(const char* str, wchar_t* wchar);
bool wchar_to_cstr(const wchar_t* wchar, char* str);

bool vfs_exist(const vpath_t path);
bool vfs_is(const vpath_t path, int type);
bool vfs_create(const vpath_t parent, const char* name, VFS_TYPE type);
bool vfs_new(const char* cstr_path, vpath_t out);
bool vfs_system_root(vpath_t out);
bool vfs_user_home(vpath_t out);
bool vfs_depth_len(const vpath_t path, uint32_t* out_depth);
bool vfs_cd(const vpath_t path);
uint32_t vfs_divide(const vpath_t path, vpath_t* out);
bool vfs_find_path(const vpath_t path, bool create_missing);
bool vfs_current_path(vpath_t out);
bool vfs_extend_path(const vpath_t parent, const char* child, vpath_t out);
uint32_t vfs_split(vpath_t in, const char* delimiter, vpath_t* out);
bool vfs_extension(const char* filename, vpath_t outext);
bool vfs_filename(vpath_t path, vpath_t out, bool remove_extension);
bool vfs_find(vpath_t path, vpath_t outpath, const char* search_name, bool recursive);

void vfs_to_unix(vpath_t path);
void vfs_to_win32(vpath_t path);

// if extension is NULL, all files will be returned
uint32_t vfs_files(const vpath_t parent, vpath_t* files, const char* extension);
uint32_t vfs_dirs(const vpath_t parent, vpath_t* dirs);

// if extension is NULL, all files will be returned
uint32_t vfs_all_files(const vpath_t parent, vpath_t* files, const char* extension);
#endif