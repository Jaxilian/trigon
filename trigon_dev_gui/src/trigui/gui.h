#ifndef TRIGON_GUI_H
#define TRIGON_GUI_H

#include <inttypes.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {
		void*		vulkan_window;
		void*		vulkan_instance;
		void*		vulkan_device;
		void*		vulkan_physical_device;
		void*		vulkan_graphics_queue;
		void*		vulkan_renderpass;
		uint32_t	vulkan_graphics_family;
		uint32_t	vulkan_image_count;
		uint32_t	vulkan_min_image_count;
	} gui_vk_info_t;

	void gui_destroy(gui_vk_info_t* info);
	void gui_init_vk(gui_vk_info_t* info);

	void gui_frame_begin();
	void gui_frame_end(void* command_buffer);

	bool gui_window_new(const char* name, bool* active);
	void gui_window_end();


#ifdef __cplusplus
}
#endif

#endif