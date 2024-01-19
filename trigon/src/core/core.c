#include "core.h"
#include <vkl/vkl.h>
#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

static GLFWwindow*		window		= NULL;
static vkl_device_t		device		= { 0 };
static vkl_swapchain_t	swapchain	= { 0 };
static vkl_state_t		state		= { 0 };
static ivec2			extent		= { 800,600 };

static void create_swap(uint32_t width, uint32_t height) {

	vkl_swapchain_info_t swap_info = {
		.device_ptr = &device,
		.prefered_present_mode = VK_PRESENT_MODE_MAILBOX_KHR,
		.surface_extent = {width,height}
	};

	vkl_swapchain_new(&swap_info, &swapchain);
}

static void glfw_framebuffer_resize_cb(GLFWwindow* window, int x, int y) {

	if (x <= 0 || y <= 0) {
		return;
	}
	extent[0] = x;
	extent[1] = y;

	create_swap((uint32_t)x, (uint32_t)y);
	signal_fire(ON_WINDOW_RESIZED_SIGNAL);
}

static void create_vulkan_instance() {
	glfwInit();

	uint32_t glfw_extc = 0;
	const char** glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_extc);

#ifdef _DEBUG
	uint32_t extc = glfw_extc + 1;
	const char** extensions = malloc(sizeof(char*) * extc);
	if (extensions == NULL) {
		printf("failed to allocate memory for extensions!\n");
		exit(1);
	}

	for (uint32_t i = 0; i < glfw_extc; i++) {
		extensions[i] = glfw_ext[i];
	}
	extensions[glfw_extc] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

	uint32_t    validation_count = 1;
	const char* validation_layers[1] = {
		"VK_LAYER_KHRONOS_validation"
	};
#else
	const char** extensions = glfw_ext;
	uint32_t extc = glfw_extc;
	validation_count = 0;
	validation_layers = NULL;
#endif

	vkl_instance_info_t info = {
		.app_name = "vge2",
		.app_version = VK_MAKE_API_VERSION(1,0,0,0),
		.eng_version = VK_MAKE_API_VERSION(1,0,0,0),
		.eng_name = "test",
		.instance_ext = extensions,
		.instance_ext_count = extc,
		.opt_validation_layers = validation_layers,
		.opt_validation_layer_count = validation_count,
		.vk_version = VK_VERSION_1_3
	};

	vkl_instance_new(&info, &device);
}

static void create_window() {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(extent[0], extent[1], "Test", NULL, NULL);
	glfwCreateWindowSurface(device.instance, window, NULL, &device.surface);
	glfwSetFramebufferSizeCallback(window, glfw_framebuffer_resize_cb);
}

static void create_vulkan_device() {

	uint32_t    device_extension_count = 1;
	const char* device_extenions[1] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	vkl_device_info_t dev_info = {
		.device_extensions = device_extenions,
		.device_extension_count = device_extension_count,
	};
	vkl_device_new(&dev_info, &device);
}



void trigon_core_init() {
	create_vulkan_instance();
	create_window();
	create_vulkan_device();

	create_swap(extent[0], extent[1]);

	vkl_state_info_t state_info = {
		.bound_device = &device,
		.bound_swapchain = &swapchain
	};

	vkl_state_new(&state_info, &state);
	signal_fire(ON_START_SIGNAL);
	

	
}

void  trigon_core_start(signal_cb draw_cb) {
	bool running = true;
	while (running) {
		glfwPollEvents();
		VkResult result = VK_FALSE;
		signal_fire(ON_UPDATE_SIGNAL);
		result = vkl_state_frame_begin(&state);
		if (result == VK_SUCCESS) {
			signal_fire(ON_DRAW_SIGNAL);
			draw_cb();
			result = vkl_state_frame_end(&state);
		}

		if (glfwWindowShouldClose(window)) {
			running = false;
		}
	}
}

void trigon_core_del() {
	vkDeviceWaitIdle(device.device);
	signal_fire(ON_QUIT_SIGNAL);
	vkl_state_del(&state);
	vkl_swapchain_del(&device, &swapchain);
	vkl_device_del(&device);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void* trigon_core_swapchain() {
	return &swapchain;
}

void* trigon_core_vkl_device() {
	return &device;
}

void* trigon_core_vkl_state() {
	return &state;
}

void trigon_core_win_extent(ivec2 _extent) {
	glm_ivec2_copy(extent, _extent);
}