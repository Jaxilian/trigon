#include "shader.h"
#include "dev/gpu/vk.h"
#include "dev/io/io.h"
#include <string.h>

static vk_shader_t* vk_shader_next() {
	for (uint32_t i = 0; i < MAX_SHADERS; i++) {
		if (ctx->shaders[i].initialized) continue;
		return &ctx->shaders[i];
	}
	validate(false, "maximum shaders reach!\n");
	return NULL;
}

static void load_shader(shader_info_t* info, vk_shader_t* shader) {
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

void shader_new(shader_info_t* info, shader_t* out) {
	memset(out, 0, sizeof(shader_t));
	out->group_count = info->group_count;
	out->name = info->shader_name;

	vk_shader_t* shader = vk_shader_next();
	load_shader(info, shader);

	vk_descriptor_set_t* sets[MAX_DESC_SETS_IN_USE] = { 0 };
	for (uint32_t i = 0; i < info->group_count; i++) {
		vk_descriptor_set_t* target = &ctx->descriptor_sets[info->groups[i]->desc_set_id];
		validate(target->initialized, "trying to fetch descriptor sets in shader_new() but %d was NULL\n", i);
		out->groups[i] = info->groups[i];
		sets[0] = target;
	}

	out->pipeline = vk_pipeline_new(sets, info->group_count, shader, NULL);
	
}

void shader_del(shader_t* shader) {
	vk_pipeline_del(shader->pipeline);
	memset(shader, 0, sizeof(shader_t));
}

void shader_use(shader_t* shader) {
	vk_pipeline_t* pipeline = vk_pipeline_get(shader->pipeline);
	vk_pipeline_bind(pipeline);

	VkDescriptorSet desc_sets[MAX_DESC_SETS_IN_USE] = { 0 };
	uint32_t dynamic_offsets[MAX_SHADER_BUFFERS] = { 0 };
	uint32_t dynamic_offset_count = 0;

	for (uint32_t i = 0; i < shader->group_count; i++) {
		shader_resource_group_t* group = shader->groups[i];
		vk_descriptor_set_t* desc_set = &ctx->descriptor_sets[group->desc_set_id];

		if (desc_set->initialized) {
			desc_sets[i] = desc_set->set;
			if (!group->has_dynamic_buffer) continue;

			for (uint32_t j = 0; j < group->resource_count; j++) {
				shader_resource_t* resource = &group->resources[j];
				if (resource->type == SHADER_DYNAMIC_BUFFER) {
					vk_buffer_t* buffer = (vk_buffer_t*)resource->data;
					uint32_t buffer_offset = (uint32_t)buffer->stride;
					dynamic_offsets[dynamic_offset_count++] = buffer_offset;
				}
			}
		}
	}

	if (dynamic_offset_count > 0) {
		vkCmdBindDescriptorSets(
			vk_command_buffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline->layout,
			0,                    // first set
			shader->group_count,   // set count
			desc_sets,             // descriptor sets
			dynamic_offset_count,  // dynamic offset count
			dynamic_offsets       // dynamic offsets
		);
	}
}