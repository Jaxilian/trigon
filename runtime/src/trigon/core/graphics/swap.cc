#include "swap.h"
#include <vulkan/vulkan.h>
#include "gpu.h"



void swapchain_t::create(void* surface) {
    _surface = surface;
}