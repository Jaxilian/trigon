#ifndef TG_WIN_H
#define TG_WIN_H

#include "tgcore/rdr/backend/vkdef.h"
#include "cstd/cstd.h"
#include "cglm/vec2.h"

#define MAIN_WINDOW_ID 0

typedef enum {
	WIN_EVENT_RESIZE,
	WIN_EVENT_DELETE,
	WIN_EVENT_CREATE,
	WIN_EVENT_COUNT
} WIN_EVENT;

typedef void (*win_event_cb_t)(uint32_t win_id, WIN_EVENT ev);

uint32_t	win_new(cstr_t name);
void		win_del(uint32_t id);

void		win_event_callback(win_event_cb_t cv);


void		win_get_extent(uint32_t id, vec2 out);
void		win_get_position(uint32_t id, vec2 out);
void		win_resize(uint32_t id, vec2 new_size);
void		win_poll_events();
float		win_get_aspect(uint32_t id);
vptr_t		win_get_handle(uint32_t id);
cstr_t*		win_extensions(uint32_t* _extc);

VkResult win_surface(uint32_t id, VkInstance instance, VkSurfaceKHR* surface);



#endif // !TG_WIN_H
