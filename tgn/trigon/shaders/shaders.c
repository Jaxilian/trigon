#include "shaders.h"

void shader_new(shader_t* shader, uint32_t SHADER_FLAGS) {

	debug_log("NEW SHADER: \n");

	if (flag_active(SHADER_FLAGS, SHADER_FLAG_USE_3D)) {
		debug_log("using 3D: yes\n");
	}
	else {
		debug_log("using 3D: no\n");
	}
}

void shader_del(shader_t* shader) {

}