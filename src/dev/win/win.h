#ifndef WIN_H
#define WIN_H
#include "dev/std/std.h"

void win_new();
void win_del();
bool win_valid();
bool win_supported();
void win_poll_events();

cstr* win_open_extensions(uint32_t* count);
void  win_close_extensions();

uint32_t	win_window_resize_connect(signal_t cb);
void		win_window_resize_disconnect(uint32_t id);


#endif // !WIN_H
