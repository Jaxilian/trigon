#include "res.h"
#include "cmn/fs.h"
#include "cmn/dict.h"
#include "cmn/cmn.h"
#include <stdlib.h>
#include "gfx/shading/gfx_shading.h"
#include "string.h"
#include "cmn/dict.h"

static dict_t shaders = { 0 };

void res_shader_load(const char* path, const char* pack) {

	if (!shaders.table) {
		dict_new(&shaders, 16);
	}

	fs_t p;
	fs_new(p, path);

	if (!fs_exist(p)) {
		debug_wrn("path: %s does not exist!\n, can't load shaders", path);
		return;
	}

}

void res_shader_build(const char* path, const char* pack) {
	
	fs_t bin = { 0 };
	res_asset_folder(pack, RESOURCE_ENUM_SHADER, bin);
	fs_mkdir(bin);

	int fc		= fs_ls(path, NULL, NULL, true, NULL);
	fs_t* fl	= malloc(sizeof(fs_t) * fc);
	assert(fl != NULL, "failed to allocate memory for shader files (fl|res_shader_build)\n");

	fs_ls(path, fl, NULL, true, NULL);

	fs_t extension	= { 0 };
	fs_t name		= { 0 };

	for (int i = 0; i < fc; i++) {

		fs_ext(extension, fl[i]);
		fs_name(name, fl[i]);

		size_t size = fs_readf(fl[i], NULL);
		char* buffer = malloc(size+1);
		fs_readf(fl[i], buffer);

		bool is_vert = strcmp(extension, "vert") == 0;

		fs_t outfile = { 0 };
		fs_new(outfile, bin);
		fs_add(outfile, name);

		size_t shader_size	= 0;
		char* shader_bin	= NULL;

		shaderc_shader_kind kind;
		const char* shader_extension = NULL;

		if (is_vert) {
			kind = shaderc_glsl_default_vertex_shader;
			shader_extension = VERT_FORMAT;
		}
		else {
			kind = shaderc_glsl_default_fragment_shader;
			shader_extension = FRAG_FORMAT;
		}

		fs_add(outfile, shader_extension);

		shader_size = gfx_compile_shader(
			buffer,
			size,
			kind,
			NULL,
			name
		);

		shader_bin = malloc(shader_size);
		shader_size = gfx_compile_shader(
			buffer,
			size,
			kind,
			shader_bin,
			name
		);

		fs_writef(
			outfile,
			shader_bin,
			shader_size
		);
		
		debug_log("compiling shader: %d/%d\n", i+1, fc);

		free(buffer);
	}

	free(fl);
}