#include "shaders.h"
#include "cmn/fs.h"

#include "res/res.h"
#include <stdlib.h>
#include "cmn/cmn.h"
#include "cmn/dict.h"

static dict_t shader_lib = { 0 };


void shader_new(shader_t* shader, shader_info_t* info) {
	if (!shader_lib.table) {
		dict_new(&shader_lib, 16);
	}

	void* ptr = dict_get(&shader_lib, info->name);
	if (ptr) {
		debug_err("shader with name %s already exists\n", info->name);
	}

	fs_t shader_folder = { 0 };
	res_asset_folder(
		info->pack,
		RESOURCE_ENUM_SHADER,
		shader_folder
	);

	VkShaderModule vertmod;
	VkShaderModule fragmod;
	// vertex

	fs_t shader_name = { 0 };
	fs_new(shader_name, shader_folder);
	fs_add(shader_name, info->name);
	fs_add(shader_name, VERT_FORMAT);

	size_t bin_size = fs_readf(shader_name, NULL);
	char* bin = malloc(bin_size+1);
	fs_readf(shader_name, bin);

	gfx_shader_new((uint32_t*)bin, bin_size, &vertmod);
	free(bin);

	// fragment

	fs_new(shader_name, shader_folder);
	fs_add(shader_name, info->name);
	fs_add(shader_name, FRAG_FORMAT);

	bin_size = fs_readf(shader_name, NULL);
	bin = malloc(bin_size + 1);
	fs_readf(shader_name, bin);

	gfx_shader_new((uint32_t*)bin, bin_size, &fragmod);
	free(bin);

    // pipeline
	gfx_pipe_info_t pipeinf = { 0 };
	pipeinf.window = info->window;
	pipeinf.fragmod = fragmod;
	pipeinf.vertmod = vertmod;
	gfx_pipe_def(&pipeinf.settings);

	gfx_pipe_layout_setup(info->sets, info->set_count, &pipeinf.settings.pipeline_layout);

	VkPipelineLayoutCreateInfo temp_info = {
	  .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	  .setLayoutCount = 0,         // Optional for now
	  .pSetLayouts = NULL,         // Optional for now
	  .pushConstantRangeCount = 0, // Optional  for now
	  .pPushConstantRanges = NULL  // Optional for now
	};


	gfx_pipe_new(&pipeinf, &shader->pipe);

	shader->name	= info->name;
	shader->pack	= info->pack;
	shader->win		= info->window;
	

	// do after pipeline creation
	vkDestroyShaderModule(gfx_dev()->device, fragmod, NULL);
	vkDestroyShaderModule(gfx_dev()->device, vertmod, NULL);


	gfx_desc_pool_setup(&shader->pipe, info->sets, info->set_count);

	dict_add(&shader_lib, info->name, shader, sizeof(shader_t));
}


void shader_del(shader_t* shader) {
	if (!shader) return;

	if (shader->pipe.pipeline) {
		vkDestroyPipeline(gfx_dev()->device, shader->pipe.pipeline, NULL);
		shader->pipe.pipeline = VK_NULL_HANDLE;
	}

	if (shader->pipe.layout) {
		vkDestroyPipelineLayout(gfx_dev()->device, shader->pipe.layout, NULL);
		shader->pipe.layout = VK_NULL_HANDLE;
	}



	dict_pop(&shader_lib, shader->name);
}


static void shader_lib_loop(const char* key, void* val) {
	shader_t* shader = (shader_t*)val;
	if (shader->pipe.pipeline) {
		vkDestroyPipeline(gfx_dev()->device, shader->pipe.pipeline, NULL);
		shader->pipe.pipeline = VK_NULL_HANDLE;
	}

	if (shader->pipe.layout) {
		vkDestroyPipelineLayout(gfx_dev()->device, shader->pipe.layout, NULL);
		shader->pipe.layout = VK_NULL_HANDLE;
	}

}

void shaders_clear() {
	dict_loop(&shader_lib, shader_lib_loop);
	dict_del(&shader_lib);
	
}