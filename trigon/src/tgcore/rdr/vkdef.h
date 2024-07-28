#ifndef TG_VK_DEF
#define TG_VK_DEF
#include "cstd/cstd.h"

#ifdef _USING_VULKAN_SDK
#include <vulkan/vulkan.h>
 
#else
typedef vptr_t VkSurfaceKHR;

#endif




#endif // !TG_VK_DEF
