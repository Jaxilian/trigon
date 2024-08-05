#include "vulkan.h"

vkcmdr_t::vkcmdr_t(vkdev_t& dev, swap_t& swp)
	:device(dev), swap(swp) {

    cmdbuffers = new VkCommandBuffer[MAX_FRAMES_IN_FLIGHT];

    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = device.cmdpool;
    info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    cassert(
        vkAllocateCommandBuffers(
            device.device,
            &info,
            cmdbuffers) == VK_SUCCESS,
        "renderer_t: failed to allocate command buffers!\n"
    );

}

vkcmdr_t::~vkcmdr_t() {
    if (!cmdbuffers) return;

    vkFreeCommandBuffers(
        device.device,
        device.cmdpool,
        MAX_FRAMES_IN_FLIGHT,
        cmdbuffers);

    delete[] cmdbuffers;
    cmdbuffers = nullptr;
}

VkCommandBuffer vkcmdr_t::cmd_begin() {
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = device.cmdpool;
    info.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(device.device, &info, &cmd);

    VkCommandBufferBeginInfo begin_inf{};
    begin_inf.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_inf.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd, &begin_inf);

    return cmd;
}

void vkcmdr_t::cmd_end(VkCommandBuffer cmd) {
    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    vkQueueSubmit(device.graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.graphics_queue);

    vkFreeCommandBuffers(device.device, device.cmdpool, 1, &cmd);
}

VkResult vkcmdr_t::frame_begin() {
    cassert(!frame_started, "frame already started!\n");

    VkResult result = swap.acquire_next(&image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swap.create();
        return result;
    }

    cassert(
        result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
        "failed to acquire swapchain image index!"
    );
    frame_started = true;

    VkCommandBuffer buff = cmdbuff();
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    cassert(
        vkBeginCommandBuffer(buff, &begin_info) == VK_SUCCESS,
        "failed to begin recording command buffer!"
    );

    VkRenderPassBeginInfo renderpass_info{};
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpass_info.renderPass = swap.renderpass;
    renderpass_info.framebuffer = swap.framebuffers[image_index];
    renderpass_info.renderArea.offset = { 0, 0 };
    renderpass_info.renderArea.extent = swap.extent;

    VkClearValue clear_values[2]{};
    clear_values[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
    clear_values[1].depthStencil = { 1.0f, 0 };

    renderpass_info.clearValueCount = 2;
    renderpass_info.pClearValues = clear_values;

    vkCmdBeginRenderPass(
        cmdbuff(),
        &renderpass_info,
        VK_SUBPASS_CONTENTS_INLINE
    );

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swap.extent.width);
    viewport.height = static_cast<float>(swap.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, swap.extent };
    vkCmdSetViewport(cmdbuff(), 0, 1, &viewport);
    vkCmdSetScissor(cmdbuff(), 0, 1, &scissor);

    return VK_SUCCESS;
};

VkResult vkcmdr_t::frame_end() {
    cassert(frame_started, "frame was not started\n");
    vkCmdEndRenderPass(cmdbuff());

    cassert(
        vkEndCommandBuffer(cmdbuff()) == VK_SUCCESS,
        "failed to record command buffer!\n"
    );

    VkCommandBuffer buffer = cmdbuff();
    VkResult result = swap.submit(&buffer, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        swap.create();
    };

    frame_started = false;

    frame_index = (frame_index + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}


