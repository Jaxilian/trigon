#include "tac.h"
#include <vstd/vfs.h>

uint32_t tac_package_count() {
	path_os_t package_folder = { 0 };
	path_get_current(&package_folder);
	path_change_dir(&package_folder, "data", &package_folder);
	path_change_dir(&package_folder, "pack", &package_folder);

	uint32_t out = 0;
	path_get_subfiles_count(&package_folder, &out);
	return out;
}

bool tac_packages_get(path_os_t* paths, const uint32_t path_count) {
	path_os_t package_folder = { 0 };
	path_get_current(&package_folder);
	path_change_dir(&package_folder, "data", &package_folder);
	path_change_dir(&package_folder, "pack", &package_folder);

	return path_get_subfiles_filter(&package_folder, paths, path_count, ".pack");
}