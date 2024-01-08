#ifndef VSTD_VFS_H
#define VSTD_VFS_H
#include <stdbool.h>
// test
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef struct {
    char data[MAX_PATH];
} path_os_t;

bool path_new(path_os_t* path, const char* path_str);

bool path_get_current(path_os_t* path);
bool path_get_parent(const path_os_t* dir, path_os_t* parent);

bool path_get_subdir_count(const path_os_t* dir, int* out);
bool path_get_subdirs(const path_os_t* dir, path_os_t* out, int out_count);

bool path_get_subfiles_count(const path_os_t* path, int* out_count);
bool path_get_subfiles(const path_os_t* path, path_os_t* files, int out_count);

bool path_get_subfiles_filter_count(const path_os_t* path, int* out_count, const char* extension);
bool path_get_subfiles_filter(const path_os_t* path, path_os_t* files, int out_count, const char* extension);

bool path_exists(const path_os_t* path);
bool path_dir_new(const path_os_t* dir_path);
bool path_file_new(const path_os_t* f_path);

bool path_set_sys_dir(const path_os_t* path);

const char* path_get_last_dir(const path_os_t* dir);
const char* path_get_ext(const path_os_t* path);
#endif