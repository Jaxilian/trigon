#include "vk.h"

bool vk_acquire_next_frame() {
    vkWaitForFences(
        ctx->device.device,
        1,
        &ctx->swapchain.in_flight_fences[ctx->swapchain.current_frame],
        VK_TRUE,
        UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(
        ctx->device.device,
        ctx->swapchain.new_swap,
        UINT64_MAX,
        ctx->swapchain.available_semaphores[ctx->swapchain.current_frame],
        VK_NULL_HANDLE,
        &ctx->vk_state.image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vk_swapchain_new();
        return false;
    }

    if (result == VK_SUBOPTIMAL_KHR) {
        vk_swapchain_new();
        return false;
    }

    return true;
}

bool vk_submit_command_buffer() {
    if (ctx->swapchain.images_in_flight[ctx->vk_state.image_index] != VK_NULL_HANDLE) {
        vkWaitForFences(ctx->device.device, 1, &ctx->swapchain.images_in_flight[ctx->vk_state.image_index], VK_TRUE, UINT64_MAX);
    }
    ctx->swapchain.images_in_flight[ctx->vk_state.image_index] = ctx->swapchain.in_flight_fences[ctx->swapchain.current_frame];

    VkCommandBuffer cmd_buffers[] = { vk_command_buffer() };

    VkSubmitInfo submitInfo = { 0 };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { ctx->swapchain.available_semaphores[ctx->swapchain.current_frame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = cmd_buffers;

    VkSemaphore signalSemaphores[] = { ctx->swapchain.finished_semaphores[ctx->swapchain.current_frame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(ctx->device.device, 1, &ctx->swapchain.in_flight_fences[ctx->swapchain.current_frame]);
    bool success = false;
    VkQueue queue = ctx->device.graphics_queue;
    VkFence fence = ctx->swapchain.in_flight_fences[ctx->swapchain.current_frame];
    success = vkQueueSubmit(queue, 1, &submitInfo, fence) == VK_SUCCESS;
    validate(success, "failed to submit draw command buffer!");


    VkPresentInfoKHR presentInfo = { 0 };
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { ctx->swapchain.new_swap };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &ctx->vk_state.image_index;

    VkResult result = vkQueuePresentKHR(ctx->device.present_queue, &presentInfo);

    ctx->swapchain.current_frame = (ctx->swapchain.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || ctx->win.resized) {
        ctx->win.resized = false;
        vk_swapchain_new();
        return false;
    }

    validate(result == VK_SUCCESS, "failed to present swap chain image!");

    return true;
}

VkCommandBuffer vk_command_buffer() {
    return ctx->device.command_buffers[ctx->vk_state.frame_index];
}

void vk_frame_begin() {
    vkDeviceWaitIdle(ctx->device.device);
    validate(!ctx->vk_state.frame_in_progress, "frame was already started!\n");
    bool success = false;

    success = vk_acquire_next_frame();
    if (!success) return;

    ctx->vk_state.frame_in_progress = true;

    VkCommandBufferBeginInfo beginInfo = { 0 };
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    success = vkBeginCommandBuffer(vk_command_buffer(), &beginInfo) == VK_SUCCESS;
    validate(success, "failed to begin recording command buffer!");
  
    if (!ctx->vk_state.frame_in_progress) {
        return;
    }

    VkRenderPassBeginInfo renderPassInfo = { 0 };
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = ctx->swapchain.renderpass;
    renderPassInfo.framebuffer = ctx->swapchain.framebuffers[ctx->vk_state.image_index];
    renderPassInfo.renderArea.offset = (VkOffset2D){ 0,0 };
    renderPassInfo.renderArea.extent = ctx->swapchain.extent;


    VkClearValue clearValues[2] = { 0 };
    clearValues[0].color = (VkClearColorValue){ 0.01f, 0.01f, 0.01f, 1.0f };
    clearValues[1].depthStencil = (VkClearDepthStencilValue){ 1.0f, 0 };

    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(vk_command_buffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = { 0 };
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)ctx->swapchain.extent.width;
    viewport.height = (float)ctx->swapchain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor = { {0, 0}, ctx->swapchain.extent };
    vkCmdSetViewport(vk_command_buffer(), 0, 1, &viewport);
    vkCmdSetScissor(vk_command_buffer(), 0, 1, &scissor);
}

void vk_frame_end() {
    vkCmdEndRenderPass(vk_command_buffer());

    if (!ctx->vk_state.frame_in_progress) return;

    bool success = vkEndCommandBuffer(vk_command_buffer()) == VK_SUCCESS;
    validate(success, "failed to record command buffer!");

    success = vk_submit_command_buffer();

    ctx->vk_state.frame_in_progress = false;
    ctx->vk_state.frame_index = (ctx->vk_state.frame_index + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkCommandBuffer vk_single_cmd_begin() {
    VkCommandBufferAllocateInfo info = {0};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = ctx->device.command_pool;
    info.commandBufferCount = 1;

    VkCommandBuffer cmd_buff;
    vkAllocateCommandBuffers(ctx->device.device, &info, &cmd_buff);

    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmd_buff, &begin_info);
    return cmd_buff;
}

void vk_single_cmd_end(VkCommandBuffer cmd_buff) {
    vkEndCommandBuffer(cmd_buff);

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buff;

    vkQueueSubmit(ctx->device.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(ctx->device.graphics_queue);

    vkFreeCommandBuffers(ctx->device.device, ctx->device.command_pool, 1, &cmd_buff);
}
