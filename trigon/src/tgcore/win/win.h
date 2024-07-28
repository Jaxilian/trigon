#ifndef TG_WIN_H
#define TG_WIN_H

#include "cstd/cstd.h"
#include "cglm/vec2.h"

uint32_t	win_new(cstr_t name);
void		win_del(uint32_t id);

void		win_get_extent(uint32_t id, vec2 out);
void		win_get_position(uint32_t id, vec2 out);
uint32_t	win_get_aspect(uint32_t id);
vptr_t		win_get_handle();

void service_win_start();
void service_win_stop();

#endif // !TG_WIN_H
