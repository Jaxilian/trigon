#ifndef TGN_DATA_H
#define TGN_DATA_H
#include "trigon/math/math.h"
#include "gfx/gfx.h"

typedef struct {
	mat4 camera_transform;
	mat4 camera_projection;

	vec4 light_direction;
	vec4 light_color;

	vec4 _empty1;
	vec4 _empty2;

	vec4 _empty3;
	vec4 _empty4;

	vec4 _empty5;
	vec4 _empty6;

} scene_data_t; //256 bytes max


void tgn_scene_data_init();
void tgn_scene_data_update(const scene_data_t* data, const win_t* win);
void tgn_scene_data_bind(VkCommandBuffer cmd, VkPipelineLayout layout, const win_t* win);
void tgn_scene_data_cleanup();



#endif