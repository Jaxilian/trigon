#include "gfxctx.h"
#include <vulkan/vulkan.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "cmn/cmn.h"

// Begin a new frame for rendering
bool gfx_frame_begin(win_t* win) {

    if (win->device->device == NULL) {
        return false;
    }

    if (win->frame_in_process) {
        return false;
    }

    win->frame_ready = false;

    uint32_t current_frame = win->swapchain.current_frame;

    // Wait for the previous frame to finish
    VkResult result = vkWaitForFences(
        win->device->device,
        1,
        &win->swapchain.in_flight_fences[current_frame],
        VK_TRUE,
        UINT64_MAX
    );
    if (result != VK_SUCCESS) {
        debug_err("Failed to wait for fences!\n");
        return false;
    }

    // Acquire the next image from the swapchain
    result = vkAcquireNextImageKHR(
        win->device->device,
        win->swapchain.new_swap,
        UINT64_MAX,
        win->swapchain.available_semaphores[current_frame],
        VK_NULL_HANDLE,
        &win->image_index
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // Swapchain is out of date (e.g., window resized), handle recreation
        gfx_swap_sync(win);
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        debug_err("Failed to acquire swapchain image!\n");
        return false;
    }

    win->frame_in_process = true;

    // Reset command buffer for recording
    vkResetCommandBuffer(win->device->cmd_buffers[current_frame], 0);

    // Begin recording commands
    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    result = vkBeginCommandBuffer(
        win->device->cmd_buffers[current_frame],
        &begin_info
    );

    if (result != VK_SUCCESS) {
        debug_err("Failed to begin command buffer!\n");
        win->frame_in_process = false;
        return false;
    }

    // Set up clear values for the color and depth buffers
    VkClearValue clear_values[2];
    clear_values[0].color.float32[0] = win->color[0];
    clear_values[0].color.float32[1] = win->color[1];
    clear_values[0].color.float32[2] = win->color[2];
    clear_values[0].color.float32[3] = win->color[3];

    clear_values[1].depthStencil.depth = 1.0f;
    clear_values[1].depthStencil.stencil = 0;

    // Begin the render pass
    VkRenderPassBeginInfo renderpass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = win->swapchain.renderpass,
        .framebuffer = win->swapchain.framebuffers[win->image_index],
        .renderArea = { {0, 0}, win->swapchain.extent },
        .clearValueCount = 2,
        .pClearValues = clear_values
    };

    vkCmdBeginRenderPass(
        win->device->cmd_buffers[current_frame],
        &renderpass_info,
        VK_SUBPASS_CONTENTS_INLINE
    );

    // Set viewport and scissor
    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)win->swapchain.extent.width,
        .height = (float)win->swapchain.extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = win->swapchain.extent
    };

    vkCmdSetViewport(
        win->device->cmd_buffers[current_frame],
        0,
        1,
        &viewport
    );

    vkCmdSetScissor(
        win->device->cmd_buffers[current_frame],
        0,
        1,
        &scissor
    );

    win->frame_ready = true;
    return win->frame_ready;
}

VkCommandBuffer gfx_cmdbuff(win_t* win) {
    uint32_t current_frame = win->swapchain.current_frame;
    return win->device->cmd_buffers[current_frame];
}

// End the current frame and submit it for presentation
VkResult gfx_frame_end(win_t* win) {

    if (!win->frame_ready) {
        return VK_NOT_READY;
    }

    if (!win->frame_in_process) {
        return VK_NOT_READY;
    }

    uint32_t current_frame = win->swapchain.current_frame;

    vkCmdEndRenderPass(win->device->cmd_buffers[current_frame]);

    VkResult result = vkEndCommandBuffer(win->device->cmd_buffers[current_frame]);
    if (result != VK_SUCCESS) {
        debug_err("Failed to record command buffer!\n");
        win->frame_in_process = false;
        return result;
    }

    // Check if the image is already in use by another frame
    if (win->swapchain.images_in_flight[win->image_index] != VK_NULL_HANDLE) {
        vkWaitForFences(
            win->device->device,
            1,
            &win->swapchain.images_in_flight[win->image_index],
            VK_TRUE,
            UINT64_MAX
        );
    }

    // Mark the image as now being in use by this frame
    win->swapchain.images_in_flight[win->image_index] = win->swapchain.in_flight_fences[current_frame];

    // Submit the command buffer
    VkSemaphore wait_semaphores[] = { win->swapchain.available_semaphores[current_frame] };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signal_semaphores[] = { win->swapchain.finished_semaphores[current_frame] };

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = wait_semaphores,
        .pWaitDstStageMask = wait_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = &win->device->cmd_buffers[current_frame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signal_semaphores
    };

    // Reset the fence for the current frame
    vkResetFences(
        win->device->device,
        1,
        &win->swapchain.in_flight_fences[current_frame]
    );

    result = vkQueueSubmit(
        win->device->graphics_queue,
        1,
        &submit_info,
        win->swapchain.in_flight_fences[current_frame]
    );

    if (result != VK_SUCCESS) {
        debug_err("Failed to submit draw command buffer!\n");
        win->frame_in_process = false;
        return result;
    }

    // Present the image to the swapchain
    VkSwapchainKHR swapchains[] = { win->swapchain.new_swap };

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signal_semaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &win->image_index,
        .pResults = NULL
    };

    result = vkQueuePresentKHR(win->device->present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Swapchain is out of date, handle recreation
        gfx_swap_sync(win);
    }
    else if (result != VK_SUCCESS) {
        debug_err("Failed to present swapchain image!\n");
        win->frame_in_process = false;
        return result;
    }

    // Advance to the next frame
    win->swapchain.current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    win->frame_in_process = false;

    return VK_SUCCESS;
}
