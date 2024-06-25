#include "sprite.h"
#include <stdlib.h>
#include <memory.h>
#include "trigon/core/utils/debug.h"

static sprite_buffer_t sprite_buffer = { 0 };

static void init() {
	sprite_buffer.max		= 10;
	sprite_buffer.length	= 0;
	sprite_buffer.buffer	= malloc(
		sizeof(sprite_t) * sprite_buffer.max
	);

	if (sprite_buffer.buffer == NULL) {
		debug_err("failed to allocate sprite_buffer memory!\n");
	}

	memset(
		sprite_buffer.buffer,
		0,
		sizeof(sprite_t) * sprite_buffer.max
	);
}

static sprite_t* get_ptr(uint32_t index) {
	return (sprite_t*)((char*)sprite_buffer.buffer + index * sizeof(sprite_t));
}

void sprite_del(sprite_t* ptr) {
	
}

sprite_t* sprite_new(uint32_t posx, uint32_t posy) {
	return NULL;
}