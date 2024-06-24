#ifndef TG_WINDOW_H
#define TG_WINDOW_H
#include "trigon/core/utils/util.h"
#include "trigon/core/graphics/vkdef.h"

typedef struct {
	float			aspect;
	uint32_t		width;
	uint32_t		height;
	void*			window_handle;
	VkSurfaceKHR	surface;
} win_t;

void		win_event_resized(voidcb_t cb);
void		win_new(win_t* ptr);
void		win_upd(win_t* ptr);
void		win_del(win_t* ptr);
cstr_t*		win_ext(uint32_t* _extc);


#endif // !TG_WINDOW_H
