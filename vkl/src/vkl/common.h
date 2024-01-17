#ifndef COMMON_H
#define COMMON_H
#include <cglm/cglm.h>
#include <vulkan/vulkan.h>

typedef struct {
	vec3 position;
	vec3 normal;
	vec3 color;
	vec2 uv;
} vertex3_t;

typedef struct {
	vec3 position;
	vec2 uv;
} vertex2_t;




#endif // !COMMON_H
