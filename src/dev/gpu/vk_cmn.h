#ifndef VK_CMN_H
#define VK_CMN_H
#include <cglm/cglm.h>

#define MAX_DESC_SETS_IN_USE 4
#define MAX_DESC_PER_SET 8
#define MAX_UNIQUE_DESC 8
#define MAX_SHADERS 16
#define MAX_SHADER_SETS 16
#define MAX_SHADER_BUFFERS MAX_DESC_PER_SET * MAX_DESC_SETS_IN_USE

typedef enum {
	SHADER_STATIC_BUFFER,
	SHADER_DYNAMIC_BUFFER,
	SHADER_SAMPLER_BUFFER
} shader_property_e;

typedef struct {
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 uv;
} vertex_t;

#endif // !VK_CMN_H
