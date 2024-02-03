#include "editor.h"
#include <trigui/gui.h>
#include <vstd/vfs.h>
#include <stdio.h>
#include <tac/tac.h>
bool win_new_asset_pack_enabled = true;

void editor_new_asset_pack() {

	if (gui_window_new("Create Asset Pack", &win_new_asset_pack_enabled)) {
		gui_text("Create new asset pack here!\n");
		if (gui_button("Import...", (float[]) { 0.0f, 0.0f })) {
			path_os_t data = { 0 };
			if (path_dir_from_explorer(&data)) {

				printf("%s\n", data.data);
			}
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