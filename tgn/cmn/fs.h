#ifndef TGN_FS_H
#define TGN_FS_H

#include <stdbool.h>
#include <stddef.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef PROJ_ROOT
// if not set in compiler! if needed, set it at runtime
#define PROJ_ROOT "\0"
#endif // !PROJECT_DIR

#define __STD_FS_MAX 240
typedef char fs_t[__STD_FS_MAX];

void fs_new(fs_t dest, const char* str);
void fs_usr(fs_t dest);
void fs_root(fs_t dest);
void fs_add(fs_t dest, const char* extra);
void fs_pop(fs_t path);
bool fs_exist(fs_t dest);
bool fs_isfile(fs_t dest);
bool fs_find(fs_t path, fs_t child);
void fs_get(fs_t path, fs_t child);
void fs_validate(fs_t path);

// returns a list of all child dirs with a optional extension, optional recursive and optional name
int  fs_ls(const fs_t path, fs_t* out, const char* exc_ext, bool recursive, const char* name);

// returns the file extension
void fs_ext(fs_t out, fs_t path);

// returns the file name
void fs_name(fs_t dest, fs_t path);

void fs_rel(fs_t dest, fs_t ancestor, fs_t descandant);
void fs_dir(fs_t dest);
void fs_app(fs_t dest);
void fs_parent(fs_t dest, fs_t path);
void fs_last(const fs_t path, fs_t out);
bool fs_cd(fs_t dest, const char* new_dest);
void fs_mkdir(fs_t dir);

// make sure malloc is added with one malloc(size_t + 1)
size_t fs_readf(fs_t file, char* buffer);
void fs_writef(fs_t file, char* buffer, size_t size);

void fs_projdir(fs_t path);

#endif