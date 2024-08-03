#include "vkdef.h"
#include <stdlib.h>

swap_support_t::~swap_support_t() {
	if (formats)		free(formats);
	if (present_modes)	free(present_modes);
}

swap_support_t::swap_support_t(u32 _format_count, u32 _present_count)
	:format_count(_format_count), present_count(_present_count){

	formats			= (VkSurfaceFormatKHR*)	malloc(
		sizeof(VkSurfaceFormatKHR) * format_count
	);

	present_modes	= (VkPresentModeKHR*)	malloc(
		sizeof(VkPresentModeKHR) * format_count
	);
}