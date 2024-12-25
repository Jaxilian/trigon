#ifndef TGN_RES_H
#define TGN_RES_H

#include "cmn/fs.h"

#define VERT_FORMAT "_vert.spv"
#define FRAG_FORMAT "_frag.spv"

#define RESOURCE_TYPE_COUNT 2
static const char* RESOURCE_TYPES[RESOURCE_TYPE_COUNT] = {
	"shaders",
	"meshes"
};

typedef enum {
	RESOURCE_ENUM_SHADER,
	RESOURCE_ENUM_MESHES
} RESOURCE_ENUM;

void res_obj_load();
void res_img_load();
void res_shader_load(const char* path, const char* pack);
void res_shader_build(const char* path, const char* pack);

void res_pack_folder(const char* pack, fs_t out);
void res_asset_folder(const char* pack, RESOURCE_ENUM type, fs_t out);


#endif // !TGN_RES_H
