#include "renderer.h"
#include "trigon/core/std/std.h"
#include "trigon/core/dev/win/window.h"
#include "trigon/core/dev/gpu/gpu.h"

VkResult renderer_t::frame_begin() {

    cassert(!frame_started, "frame already started!\n");

    VkResult result = swap_t::ref().acquire_next(&image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swap_t::ref().create();
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
    renderpass_info.renderPass = swap_t::ref().renderpass;
    renderpass_info.framebuffer = swap_t::ref().framebuffers[image_index];
    renderpass_info.renderArea.offset = { 0, 0 };
    renderpass_info.renderArea.extent = swap_t::ref().extent;

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
    viewport.width = static_cast<float>(swap_t::ref().extent.width);
    viewport.height = static_cast<float>(swap_t::ref().extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, swap_t::ref().extent };
    vkCmdSetViewport(cmdbuff(), 0, 1, &viewport);
    vkCmdSetScissor(cmdbuff(), 0, 1, &scissor);

	return VK_SUCCESS;
}

VkResult renderer_t::frame_end() {

    cassert(frame_started, "frame was not started\n");
    vkCmdEndRenderPass(cmdbuff());

    cassert(
        vkEndCommandBuffer(cmdbuff()) == VK_SUCCESS,
        "failed to record command buffer!\n"
    );

    VkCommandBuffer buffer = cmdbuff();
    VkResult result = swap_t::ref().submit(&buffer, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        swap_t::ref().create();
    };

    frame_started = false;

    frame_index = (frame_index + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}