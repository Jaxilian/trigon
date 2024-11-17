#include "tests.h"
#include "trigon/trigon.h"
#include "trigon/shaders/shaders.h"

static shader_t font_shader = { 0 };

void test_shader_new() {
	uint32_t shader_flags = 0;
	flag_set(shader_flags, SHADER_FLAG_USE_3D);

	shader_new(&font_shader, shader_flags);

}

void test_shader_del() {
	shader_del(&font_shader);
}