#ifndef TG_SHADER_H
#define TG_SHADER_H

#include <inttypes.h>

#define MAX_DESC_PER_SET 8



typedef enum {
	SHADER_STAGE_FRAGMENT,
	SHADER_STAGE_VERTEX
} SHADER_STAGE;

typedef enum {
	SHADER_STATIC_BUFFER,
	SHADER_DYNAMIC_BUFFER,
	SHADER_SAMPLER_BUFFER
} SHADER_PROP;

typedef struct {
	uint32_t	location;
	SHADER_PROP	property;
	uint32_t	count;
} shader_binding_t;

typedef struct {
	char vertex_path[260];
	char frag_path[260];
} shader_codepaths_t;

typedef struct {
	shader_codepaths_t	paths;
} shader_info_t;

uint32_t shader_new(const shader_info_t* info);
uint32_t shader_add_resource(uint32_t shaderid, shader_binding_t binding);
void	 shader_compile(uint32_t id);
void	 shader_del(uint32_t id);

#endif // !TG_SHADER_H
