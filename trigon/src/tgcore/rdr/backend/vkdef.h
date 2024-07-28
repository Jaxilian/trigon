#ifndef TG_VK_DEF
#define TG_VK_DEF
#include "cstd/cstd.h"

#ifdef _USING_VULKAN_SDK
#include <vulkan/vulkan.h>
 
#else
typedef vptr_t VkSurfaceKHR;
typedef vptr_t VkSurfaceKHR;
typedef vptr_t VkInstance;
typedef vptr_t VkDebugUtilsMessengerEXT;
typedef vptr_t VkPhysicalDevice;
typedef vptr_t VkCommandPool;
typedef vptr_t VkDevice;
typedef vptr_t VkQueue;
typedef vptr_t VkCommandBuffer;
typedef vptr_t VkSwapchainKHR;
typedef vptr_t VkImage;
typedef vptr_t VkImageView;
typedef vptr_t VkFormat;
typedef vptr_t VkExtent2D;
typedef vptr_t VkRenderPass;
typedef vptr_t VkDeviceMemory;
typedef vptr_t VkFramebuffer;
typedef vptr_t VkSemaphore;
typedef vptr_t VkFence;
typedef vptr_t VkPresentModeKHR;
typedef vptr_t VkShaderModule;
typedef vptr_t VkPipelineLayout;
typedef vptr_t VkPipeline;
typedef vptr_t VkSurfaceFormatKHR;
typedef vptr_t VkBuffer;
typedef uint32_t VkResult;
typedef uint32_t VkMemoryPropertyFlags;
typedef uint32_t VkDeviceSize;
typedef uint32_t VkBufferUsageFlags;
typedef uint32_t VkImageTiling;
typedef uint32_t VkImageUsageFlags;
typedef uint32_t VkImageLayout;
#endif




#endif // !TG_VK_DEF
