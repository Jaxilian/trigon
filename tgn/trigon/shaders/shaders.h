#ifndef TGN_SHADERS_H
#define TGN_SHADERS_H
#include <inttypes.h>
#include "cmn/cmn.h"

#define SHADER_FLAG_USE_3D (1 << 0)


typedef struct {
	int x;
} shader_t;

void shader_new(shader_t* shader, uint32_t SHADER_FLAGS);
void shader_del(shader_t* shader);

#endif // !TGN_SHADERS_H
