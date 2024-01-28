#ifndef TRIGON_CORE_H
#define TRIGON_CORE_H
#include "signal.h"
#include <cglm/cglm.h>

typedef enum {
	CORE_FLAG_DROP_ITEM = 0x01
} CORE_FLAG;

void	trigon_core_init(uint32_t flags);
void	trigon_core_start(signal_cb update_cb, signal_cb draw_cb, signal_cb ui_draw_cb);
void	trigon_core_del();
void*	trigon_core_swapchain();
void*	trigon_core_vkl_device();
void*	trigon_core_vkl_state();
void	trigon_core_win_extent(ivec2 extent);
void*	trigon_core_window();
void	trigon_core_toggle_flag(CORE_FLAG _flag, bool enabled);
bool	trigon_core_has_flag(CORE_FLAG flag);

extern inline double time_dt();

#endif // !TRIGON_CORE_H
