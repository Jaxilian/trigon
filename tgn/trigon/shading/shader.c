#include "shader.h"
#include "gfx/gfx.h"
#include "gfx/shading/gfx_shading.h"
#include <stdio.h>
#include "cmn/fs.h"

void shader_new(shader_t* shader, shader_info_t* info) {
	fs_t fs = { 0 };
	fs_dir(fs);
	fs_mkdir(fs, "shaders");
	printf("%s\n", fs);
}


void shader_del(shader_t* shader) {

}
