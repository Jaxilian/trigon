#ifndef TGN_ASSETS_H
#define TGN_ASSETS_H

#include <stdbool.h>

//currently not possibly to build assets in release mode
void tgn_assets_load(const char* pack, bool build_assets);
void tgn_assets_unload(const char* pack);

#endif // !TGN_ASSETS_H
