#include "gfx_shading.h"
#include <stdio.h>
#include <memory.h>

size_t gfx_compile_shader(
	char* src,
	size_t size,
	shaderc_shader_kind kind,
	char* out,
	const char* filename

) {
	shaderc_compiler_t compiler = shaderc_compiler_initialize();
	if (!compiler) {
		printf("failed to start vulkan -> shaderc\n");
		return -1;
	}

	shaderc_compile_options_t options = shaderc_compile_options_initialize();

	shaderc_compile_options_set_target_env(
		options,
		shaderc_target_env_vulkan,
		shaderc_env_version_vulkan_1_3
	);

	shaderc_compilation_result_t result = VK_SUCCESS;
	result = shaderc_compile_into_spv(
		compiler,
		src,
		size,
		kind,
		filename,
		"main",
		options
	);

	shaderc_compiler_release(compiler);
	shaderc_compile_options_release(options);

	if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success) {
		fprintf(stderr, "Compilation failed: %s\n", shaderc_result_get_error_message(result));
		shaderc_result_release(result);
		return -2;
	}
	const char* shader_data = shaderc_result_get_bytes(result);
	size_t len = shaderc_result_get_length(result);

	if (out) {
		memcpy(out, shader_data, len);
	}

	shaderc_result_release(result);
	return len;
}