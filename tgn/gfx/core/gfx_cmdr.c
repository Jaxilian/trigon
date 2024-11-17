#include "gfxctx.h"

VkCommandBuffer gfx_cmd_begin() {
    VkCommandBufferAllocateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = gfx_device_get(NULL)->cmd_pool,
        .commandBufferCount = 1
    };

    VkCommandBuffer cmd_buff;
    vkAllocateCommandBuffers(
        gfx_device_get(NULL)->device,
        &info,
        &cmd_buff
    );

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    vkBeginCommandBuffer(cmd_buff, &begin_info);
    return cmd_buff;
}

void gfx_cmd_end(VkCommandBuffer cmd_buff) {
    vkEndCommandBuffer(cmd_buff);

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd_buff
    };

    vkQueueSubmit(
        gfx_device_get(NULL)->graphics_queue,
        1,
        &submit_info,
        VK_NULL_HANDLE
    );

    vkQueueWaitIdle(gfx_device_get(NULL)->graphics_queue);

    vkFreeCommandBuffers(
        gfx_device_get(NULL)->device,
        gfx_device_get(NULL)->cmd_pool,
        1,
        &cmd_buff
    );
}