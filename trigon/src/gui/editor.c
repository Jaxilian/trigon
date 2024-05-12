#include "editor.h"
#include <trigui/gui.h>
#include <vstd/vfs.h>
#include <stdio.h>
#include <tac/tac.h>

bool win_new_asset_pack_enabled = false;
path_os_t path_buffer = { 0 };
tac_gltf_info_t gltf_buffer = { 0 };

void editor_new_asset_pack() {
	if (gui_window_new("Create Asset Pack", &win_new_asset_pack_enabled)) {

		if (gui_button("Import...", (float[]) { 0.0f, 0.0f })) {
			if (path_dir_from_explorer(&path_buffer)) {

				printf("%s\n", path_buffer.data);
				tac_read_gltf(path_buffer.data, &gltf_buffer);
			}
		}

		gui_same_line(0.0f, -1.0f);
		gui_text(path_buffer.data);

		for (uint32_t i = 0; i < gltf_buffer.mesh_count; i++) {
			gui_text("[ID:%d]:", i);
			gui_same_line(0.0f, -1.0f);
			char label[64] = "[mesh:%d]";
			sprintf(label, "[mesh:%d]", i);

			gui_text_input(label, "hint!", gltf_buffer.mesh_buffer[i].name, 64);
		}

		const char* pop_id = "popup";
		if (gltf_buffer.mesh_count > 0) {
			if (gui_button("save to", (float[]) { 0.0f, 0.0f })) gui_popup_open(pop_id);
			
		}

		if(gui_popup_begin(pop_id, NULL)) {
			
			uint32_t pack_count = tac_package_count();
			gui_text("package count: %d", pack_count);
			path_os_t paths[64] = { 0 };
			tac_packages_get(paths, pack_count);

			for (uint32_t i = 0; i < pack_count; i++) {
				const char* name = path_get_last_dir(&paths[i]);

				if (gui_button(name, (float[]) { 0.0f, 0.0f })) {

				}
			}


	
			if (gui_button("cancel", (float[]) { 0.0f, 0.0f })) {
				gui_popup_close();
			}
			gui_popup_end();
		}

	}
	gui_window_end();
}

void editor_draw() {
	bool active = true;

	if (gui_main_menu_begin()) {
		if (gui_menu_begin("File", true)) {

			gui_menu_end();
		}
		if (gui_menu_begin("Assets", true)) {

			bool is_selected = false;
			if (gui_menu_item("New Pack", NULL, &is_selected, true)) {
				win_new_asset_pack_enabled = !win_new_asset_pack_enabled;
			}
			if (gui_menu_item("Import", NULL, &is_selected, true)) {

			}
			gui_menu_end();
		}
		gui_main_menu_end();
	}

	if (win_new_asset_pack_enabled) editor_new_asset_pack();
}