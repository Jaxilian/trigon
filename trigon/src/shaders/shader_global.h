#ifndef TRIGON_SHADER_GLOBAL
#define TRIGON_SHADER_GLOBAL
#include <inttypes.h>
#include <cglm/cglm.h>
typedef struct {
	vec2 win_extent;
	mat4 camera[2];

} shader_global_data_t;

void shader_global_data_get(shader_global_data_t* out);
void shader_global_data_set(shader_global_data_t* in);
void* shader_global_data_ptr(); //returns vkl_descriptor_set_t*
void shader_global_data_del();

#endif // !TRIGON_SHADER_GLOBAL
