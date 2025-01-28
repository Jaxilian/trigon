#include "cmn/cmn.h"
#include "assets.h"
#include "res/res.h"
#include "cmn/fs.h"
#include <string.h>
#include <inttypes.h>


static void build(const char* pack) {
	fs_t root = { 0 };

	fs_projdir(root);

	if (strlen(root) <= 0) {
		debug_err(
			"trying to build assets but "
			"fs_custom_projdir and PROJ_DIR was NULL\n"
		);
	}

	fs_t res_path = {"res"};
	if (!fs_cd(root, res_path)) {
		debug_err("%s does not have a sub dir called \"res\"\n", root);
	}

	if (!fs_cd(root, pack)) {
		debug_err("%s does not have a sub dir called \"%s\"\n", root, pack);
	}

	for (uint32_t i = 0; i < RESOURCE_TYPE_COUNT; i++) {
		fs_t res_path = { 0 };

		fs_new(res_path, root);

		if (fs_cd(res_path, RESOURCE_TYPES[i])) {
			res_shader_build(res_path, pack);
		}
	}
}

void tgn_assets_load(const char* pack, bool build_assets) {
#ifdef _DEBUG
	if (build_assets) build(pack);
#endif


	
}

void tgn_assets_unload(const char* pack) {

}