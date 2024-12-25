#ifndef TGN_RES_H
#define TGN_RES_H


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


#endif // !TGN_RES_H
