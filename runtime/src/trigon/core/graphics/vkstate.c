#include "vkdef.h"
#include <memory.h>
#include "trigon/core/utils/debug.h"
#include <stdbool.h>

VkResult vkstate_next_frame(vkstate_t* state) {
    vkWaitForFences(
        state->device->device,
        1,
        &state->swapchain->in_flight_fences[
            state->swapchain->current_frame
        ],
        VK_TRUE,
        UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(
        state->device->device,
        state->swapchain->new_swap,
        UINT64_MAX,
        state->swapchain->available_semaphores[
            state->swapchain->current_frame
        ],
        VK_NULL_HANDLE,
        &state->image_index);

    return result;
}

VkCommandBuffer vkstate_command_buffer(vkstate_t* state) {
    return state->device->cmd_buffers[state->frame_index];
}

VkResult vkstate_submit_commands(vkstate_t* state) {
    if (state->swapchain->images_in_flight[state->image_index] != VK_NULL_HANDLE) {
        vkWaitForFences(
            state->device->device,
            1,
            &state->swapchain->images_in_flight[state->image_index],
            VK_TRUE, UINT64_MAX
        );
    }
    state->swapchain->images_in_flight[state->image_index] = state->swapchain->in_flight_fences[
        state->swapchain->current_frame
    ];

    VkCommandBuffer cmd_buffers[] = {
        vkstate_command_buffer(state)
    };

    VkSemaphore await_semaphores[] = {
        state->swapchain->available_semaphores[state->swapchain->current_frame]
    };
    VkPipelineStageFlags await_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore signal_semaphores[] = {
        state->swapchain->finished_semaphores[state->swapchain->current_frame]
    };

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = await_semaphores,
        .pWaitDstStageMask = await_stages,
        .commandBufferCount = 1,
        .pCommandBuffers = cmd_buffers,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signal_semaphores
    };

    vkResetFences(
        state->device->device,
        1,
        &state->swapchain->in_flight_fences[state->swapchain->current_frame]
    );

    VkQueue queue = state->device->graphics_queue;
    VkFence fence = state->swapchain->in_flight_fences[
        state->swapchain->current_frame
    ];

    VkResult result = vkQueueSubmit(
        queue,
        1,
        &submit_info, fence
    );

    if (result != VK_SUCCESS) {
        debug_err("failed to submit draw command buffer!\n");
        return result;
    }

    VkSwapchainKHR swapchains[] = {
        state->swapchain->new_swap
    };

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signal_semaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &state->image_index,
    };

    result = vkQueuePresentKHR(
        state->device->present_queue,
        &present_info
    );

    state->swapchain->current_frame = (state->swapchain->current_frame + 1)
        % MAX_FRAMES_IN_FLIGHT;

    return result;
}

VkCommandBuffer vkstate_single_cmd(vkstate_t* state) {
    VkCommandBufferAllocateInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = state->device->cmd_pool,
        .commandBufferCount = 1
    };

    VkCommandBuffer cmd_buff;
    vkAllocateCommandBuffers(
        state->device->device,
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

void vkstate_single_cmd_submit(vkstate_t* state, VkCommandBuffer cmd_buff) {
    vkEndCommandBuffer(cmd_buff);

    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd_buff
    };

    vkQueueSubmit(
        state->device->graphics_queue,
        1,
        &submit_info,
        VK_NULL_HANDLE
    );

    vkQueueWaitIdle(state->device->graphics_queue);

    vkFreeCommandBuffers(
        state->device->device,
        state->device->cmd_pool,
        1,
        &cmd_buff
    );
}

VkResult vkstate_frame_begin(vkstate_t* state) {
    if (state->frame_in_progress) {
        debug_err("vkstate_frame_begin: frame was already started!\n");
    }

    VkResult result = vkstate_next_frame(state);
    if (result != VK_SUCCESS) return result;

    state->frame_in_progress = true;

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    result = vkBeginCommandBuffer(
        vkstate_command_buffer(state),
        &begin_info
    );

    if (result != VK_SUCCESS) {
        state->frame_in_progress = false;
        debug_err("failed to begin recording command buffer!\n");
        return result;
    }

    VkClearValue clear_values[2] = { 0 };
    clear_values[0].color = (VkClearColorValue){
        0.01f,
        0.01f,
        0.01f,
        1.0f
    };

    clear_values[1].depthStencil = (VkClearDepthStencilValue){
        1.0f,
        0
    };

    VkRenderPassBeginInfo renderpass_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = state->swapchain->renderpass,
        .framebuffer = state->swapchain->framebuffers[
            state->image_index
        ],
        .renderArea.offset = (VkOffset2D){ 0,0 },
        .renderArea.extent = state->swapchain->extent,
        .clearValueCount = 2,
        .pClearValues = clear_values
    };

    vkCmdBeginRenderPass(
        vkstate_command_buffer(state),
        &renderpass_info,
        VK_SUBPASS_CONTENTS_INLINE
    );

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)state->swapchain->extent.width,
        .height = (float)state->swapchain->extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        {0, 0},
        state->swapchain->extent
    };

    vkCmdSetViewport(
        vkstate_command_buffer(state),
        0,
        1,
        &viewport
    );

    vkCmdSetScissor(
        vkstate_command_buffer(state),
        0,
        1,
        &scissor
    );

    return result;
}

VkResult vkstate_frame_end(vkstate_t* state) {
    vkCmdEndRenderPass(
        vkstate_command_buffer(state)
    );

    if (!state->frame_in_progress) return VK_FALSE;

    VkResult result = vkEndCommandBuffer(
        vkstate_command_buffer(state)
    );

    if (result != VK_SUCCESS) {
        debug_err("failed to end command buffer!\n");
        return result;
    }

    result = vkstate_submit_commands(state);

    state->frame_in_progress = false;
    state->frame_index = (state->frame_index + 1)
        % MAX_FRAMES_IN_FLIGHT;

    return result;
}

void vkstate_new(vkstate_t* state, vkdevice_t* device, vkswapchain_t* swap) {
    memset(state, 0, sizeof(vkstate_t));
    state->device = device;
    state->swapchain = swap;
}

void vkstate_del(vkstate_t* state) {
    memset(state, 0, sizeof(vkstate_t));
}
