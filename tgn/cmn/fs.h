#ifndef TGN_FS_H
#define TGN_FS_H

#include <stdbool.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define __STD_FS_MAX 240
#define __STD_FS_TAG_MAX 32
#define __STD_FS_TAG_COUNT 16
typedef char fs_t[__STD_FS_MAX];

void fs_usr(fs_t dest);
void fs_root(fs_t dest);
void fs_add(fs_t dest, fs_t extra);
void fs_pop(fs_t path);
bool fs_exist(fs_t dest);
bool fs_isfile(fs_t dest);
bool fs_find(fs_t path, fs_t child);
void fs_get(fs_t path, fs_t child);
void fs_validate(fs_t path);
int  fs_ls(fs_t path, fs_t* out, const char* exc_ext, bool recursive);
void fs_ext(fs_t dest, fs_t path);
void fs_name(fs_t dest, fs_t path);
void fs_rel(fs_t dest, fs_t ancestor, fs_t descandant);
void fs_dir(fs_t dest);
void fs_app(fs_t dest);
void fs_parent(fs_t dest, fs_t path);
void fs_cd(fs_t dest, fs_t new_dest);
void fs_mkdir(fs_t dir, fs_t name);

#endif