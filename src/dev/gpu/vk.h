#ifndef T_VK_H
#define T_VK_H
#include "dev/std/std.h"
#include "dev/mem/mem.h"
#include "dev/mem/core_ctx_def.h"

swap_support_t* vk_open_swap_support(VkPhysicalDevice device);
void vk_close_swap_support(swap_support_t* support);

void vk_device_new();
void vk_device_del();



void vk_new();
void vk_del();

#endif