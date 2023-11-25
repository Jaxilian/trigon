#ifndef WIN_H
#define WIN_H
#include "dev/std/std.h"

void win_new();
void win_del();
bool win_valid();
bool win_supported();
void win_poll_events();
void win_width();
void win_height();

cstr* win_open_extensions(uint32_t* count);
void  win_close_extensions();
#endif // !WIN_H
