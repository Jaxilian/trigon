#include "shader.h"
#include "dev/gpu/vk.h"
#include "dev/io/io.h"
#include <string.h>

vk_shader_t* vk_shader_next() {
	for (uint32_t i = 0; i < MAX_SHADERS; i++) {
		if (ctx->shaders[i].initialized) continue;
		return &ctx->shaders[i];
	}
	validate(false, "maximum shaders reach!\n");
	return NULL;
}

void load_shader(shader_info_t* info, vk_shader_t* shader) {
	char vert_path_buffer[255] = { 0 };
	char frag_path_buffer[255] = { 0 };

	{
		cstr shaders_path = path_get_path("data\\shaders");

		char vertex_name[64] = { 0 };
		strcpy(vertex_name, info->shader_name);
		strcat(vertex_name, "_vert.spv");
		
		cstr full_path = path_get_file(shaders_path, vertex_name);
		strcpy(vert_path_buffer, full_path);
	}

	{
		cstr shaders_path = path_get_path("data\\shaders");

		char frag_name[64] = { 0 };
		strcpy(frag_name, info->shader_name);
		strcat(frag_name, "_frag.spv");

		cstr full_path = path_get_file(shaders_path, frag_name);
		strcpy(frag_path_buffer, full_path);
	}
	
	vk_shader_new(shader, vert_path_buffer, frag_path_buffer);
}

void shader_new(shader_info_t* info) {
	vk_shader_t* shader = vk_shader_next();
	load_shader(info, shader);


	//vk_descriptor_new();
	//vk_descriptor_set_new();

}

void shader_del() {

}