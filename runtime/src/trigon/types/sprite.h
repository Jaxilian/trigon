#ifndef TG_SPRITE_H
#define TG_SPRITE_H
#include <inttypes.h>

typedef struct {
	uint32_t queue_id;
} sprite_t;

typedef struct {
	sprite_t*	buffer;
	uint32_t	length;
	uint32_t	max;
} sprite_buffer_t;

sprite_t* sprite_new(uint32_t posx, uint32_t posy);
void	  sprite_del(sprite_t* ptr);

#endif // !TG_SPRITE_H
