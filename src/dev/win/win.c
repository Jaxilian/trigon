#include "win.h"
#include "dev/mem/core_ctx_def.h"
#include "dev/mem/mem.h"
#include <string.h>

#ifdef _DEBUG
static void glfw_error_callback(int code, cstr msg) {
	validate(false, msg);
}
#endif

static cstr*	extensions = NULL;

bool win_supported() {
	bool initialized = ctx->win.initialized;
	if (!initialized) {
		validate(glfwInit(), "failed to initialize glfw");
	}

	return glfwVulkanSupported();
}

bool win_valid() {
	return ctx->win.initialized && glfwVulkanSupported();
}

void win_new() {
	validate(win_supported(), "vulkan not supported on device");
	cstr name = ctx->app_data.name;
	if (!name) {
		name = "new project";
	}

#ifdef _DEBUG
	glfwSetErrorCallback(glfw_error_callback);
#endif

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	ctx->win.handle = glfwCreateWindow(800, 600, name, NULL, NULL);
	validate(ctx->win.handle != NULL, "failed to create window");

	ctx->win.running = true;
}

void win_del() {
	glfwDestroyWindow(ctx->win.handle);
	glfwTerminate();
	memset(&ctx->win, 0, sizeof(vk_win_t));
}

void win_width() {

}

void win_height() {

}

void win_poll_events() {
	glfwPollEvents();
	if (glfwWindowShouldClose(ctx->win.handle)){
		ctx->win.running = false;
	}
}

cstr* win_open_extensions(uint32_t* count) {
#ifdef _DEBUG
	uint32_t glfw_extc = 0;
	cstr* glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_extc);

	uint32_t extc = glfw_extc + 1;
	extensions = mem_alloc(sizeof(char*) * extc);

	for (uint32_t i = 0; i < glfw_extc; i++) {
		extensions[i] = glfw_ext[i];
	}

	extensions[glfw_extc] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	*count = extc;
	return extensions;
#else
	retrun glfwGetRequiredInstanceExtensions(count);
#endif
}

void  win_close_extensions() {
#ifdef _DEBUG
	mem_free(extensions);
#endif
}