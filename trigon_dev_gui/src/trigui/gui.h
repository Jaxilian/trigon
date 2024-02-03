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
	void gui_apply_style();

	bool gui_main_menu_begin();
	void gui_main_menu_end();

	bool gui_menu_begin(const char* name, bool active);
	void gui_menu_end();

	bool gui_menu_item(const char* name, const char* shortcut, bool* is_selected, bool active);

	void gui_frame_begin();
	void gui_frame_end(void* command_buffer);

	bool gui_window_new(const char* name, bool* active);
	void gui_window_end();

	void gui_text(const char* content, ...);
	bool gui_check_box(const char* label, bool* value);
	bool gui_button(const char* label, float extent[2]);


#ifdef __cplusplus
}
#endif

#endif