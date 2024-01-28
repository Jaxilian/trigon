#ifndef TRIGON_SHADMAN_H
#define TRIGON_SHADMAN_H

#include <cglm/mat4.h>

typedef struct {
	mat4 matrices[64];
} shader_simple_data_t;

void shader_simple_new();
void shader_simple_del();
uint32_t shader_simple_new_instance();
void shader_simple_draw(uint32_t mesh);
void shader_simple_set_instance(uint32_t id, mat4 data);
void shader_simple_submit_instances();

#endif // !TRIGON_SHADMAN_H
