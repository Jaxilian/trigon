#include "shaders.h"
#include "cmn/fs.h"
#include "gfx/pipe/gfx_pipe.h"
#include "res/res.h"
#include <stdlib.h>
#include "cmn/cmn.h"

void shader_new(shader_t* shader, shader_info_t* info) {

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
	gfx_pipe_t pipe = { 0 };
	gfx_pipe_info_t pipeinf = { 0 };
	pipeinf.window = info->window;
	pipeinf.fragmod = fragmod;
	pipeinf.vertmod = vertmod;
	gfx_pipe_def(&pipeinf.settings);
	gfx_pipe_new(&pipeinf, &pipe);

	// do lastly
	vkDestroyShaderModule(gfx_dev()->device, fragmod, NULL);
	vkDestroyShaderModule(gfx_dev()->device, vertmod, NULL);
}


void shader_del(shader_t* shader) {

}
