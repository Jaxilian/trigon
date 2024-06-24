#ifndef TG_VKDEF_H
#define TG_VKDEF_H

#ifdef _VULKAN_EXPOSED
#include <vulkan/vulkan.h>
#else
typedef void* VkSurfaceKHR;
#endif

#endif // !TG_VKDEF_H
