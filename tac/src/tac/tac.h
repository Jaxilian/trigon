#ifndef TAC_IMP_H //  TRIGON ASSET CREATOR : IMPORTER
#define TAC_IMP_H 
#include "tac_def.h"

bool tac_read_gltf(const char* path, tac_gltf_info_t* out);

uint32_t tac_package_count();
bool tac_packages_get(path_os_t* paths, const uint32_t path_count);
#endif