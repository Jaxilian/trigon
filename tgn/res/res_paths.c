#include "res.h"

void res_pack_folder(const char* pack, fs_t path) {
	fs_dir(path);
	fs_add(path,"bin/");
	fs_add(path, pack);
	fs_add(path, "/");
	fs_validate(path);
}

void res_asset_folder(const char* pack, RESOURCE_ENUM type, fs_t out) {
	res_pack_folder(pack, out);
	fs_add(out, RESOURCE_TYPES[type]);
	fs_add(out, "/");
	fs_validate(out);
}