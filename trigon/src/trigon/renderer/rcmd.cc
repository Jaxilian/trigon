#include "renderer.h"
#include "trigon/core/std/std.h"
#include "trigon/core/dev/win/window.h"
#include "trigon/core/dev/gpu/gpu.h"



void renderer_t::create_cmdr() {
    destroy_cmdr();

    cmdbuffers = new VkCommandBuffer[MAX_FRAMES_IN_FLIGHT];
 
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = vgpu_t::ref().cmdpool;
    info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    
    cassert(
        vkAllocateCommandBuffers(
            vgpu_t::ref().handle,
            &info,
            cmdbuffers) == VK_SUCCESS,
        "renderer_t: failed to allocate command buffers!\n"
    );

}

void renderer_t::destroy_cmdr() {
    if (!cmdbuffers) return;

    vkFreeCommandBuffers(
        vgpu_t::ref().handle,
        vgpu_t::ref().cmdpool,
        MAX_FRAMES_IN_FLIGHT,
        cmdbuffers);

    delete[] cmdbuffers;
    cmdbuffers = nullptr;
}


VkCommandBuffer 
renderer_t::cmd_begin() {

    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = vgpu_t::ref().cmdpool;
    info.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(vgpu_t::ref().handle, &info, &cmd);

    VkCommandBufferBeginInfo begin_inf{};
    begin_inf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_inf.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &begin_inf);

    return cmd;
}

void  
renderer_t::cmd_end(VkCommandBuffer cmd) {
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(vgpu_t::ref().graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vgpu_t::ref().graphics_queue);

    vkFreeCommandBuffers(vgpu_t::ref().handle, vgpu_t::ref().cmdpool, 1, &cmd);
}