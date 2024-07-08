#ifndef TG_RENDERER_H
#define TG_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "trigon/core/graphics/win.h"
#include "trigon/core/graphics/vkdef.h"

typedef void* rendr_cmd_t;

typedef struct {
	win_t			window;
	vkdevice_t		device;
	vkswapchain_t	swapchain;
	vkstate_t		state;
} rendr_t;


void rendr_new(rendr_t* rendr);
void rendr_upd(rendr_t* rendr);
void rendr_bgn(rendr_t* rendr);
void rendr_end(rendr_t* rendr);
void rendr_del(rendr_t* rendr);

rendr_cmd_t rendr_new_single_cmd();
void		rendr_end_single_cmd(rendr_cmd_t cmd);

rendr_t* rendr_get();

#ifdef __cplusplus
}
#endif


#endif // !TG_RENDERER_H
