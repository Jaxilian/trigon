#ifndef TRIGON_CORE_H
#define TRIGON_CORE_H
#include "signal.h"
#include <cglm/cglm.h>

void	trigon_core_init();
void	trigon_core_start(signal_cb draw_cb, signal_cb ui_draw_cb);
void	trigon_core_del();
void*	trigon_core_swapchain();
void*	trigon_core_vkl_device();
void*	trigon_core_vkl_state();
void	trigon_core_win_extent(ivec2 extent);
void*	trigon_core_window();
#endif // !TRIGON_CORE_H
