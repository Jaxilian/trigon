#include "vkl.h"
#include <memory.h>

VkResult vkl_state_next_frame(vkl_state_t* state) {
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

VkCommandBuffer vkl_state_command_buffer(vkl_state_t* state) {
    vkl_device_t* dev = state->device;

    return dev->command_buffers[state->frame_index];
}

VkResult vkl_state_submit_commands(vkl_state_t* state) {
    vkl_swapchain_t* swap = state->swapchain;
    vkl_device_t* dev = state->device;

    if (swap->images_in_flight[state->image_index] != VK_NULL_HANDLE) {
        vkWaitForFences(
            dev->device,
            1,
            &swap->images_in_flight[state->image_index],
            VK_TRUE, UINT64_MAX
        );
    }
    swap->images_in_flight[state->image_index] = swap->in_flight_fences[
        swap->current_frame
    ];

    VkCommandBuffer cmd_buffers[] = { 
        vkl_state_command_buffer(state)
    };

    VkSemaphore await_semaphores[] = {
        swap->available_semaphores[swap->current_frame] 
    };
    VkPipelineStageFlags await_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    
    VkSemaphore signal_semaphores[] = { 
        swap->finished_semaphores[swap->current_frame] 
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
        dev->device,
        1,
        &swap->in_flight_fences[swap->current_frame]
    );

    VkQueue queue   = dev->graphics_queue;
    VkFence fence   = swap->in_flight_fences[
        swap->current_frame
    ];

    VkResult result = vkQueueSubmit(
        queue,
        1,
        &submit_info, fence
    );

    if (result != VK_SUCCESS) {
        vkl_error("failed to submit draw command buffer!\n",
            ERROR_WARNING
        );
        return result;
    }

    VkSwapchainKHR swapchains[] = { 
        swap->new_swap 
    };

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = signal_semaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &state->image_index,
    };


    result = vkQueuePresentKHR(
        dev->present_queue,
        &present_info
    );

    swap->current_frame = (swap->current_frame + 1)
        % MAX_FRAMES_IN_FLIGHT;

    return result;
}

VkCommandBuffer vkl_state_single_cmd(vkl_state_t* state) {
    VkCommandBufferAllocateInfo info = { 
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = state->device->command_pool,
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

void vkl_state_single_cmd_submit(vkl_state_t* state, VkCommandBuffer cmd_buff) {
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
        state->device->command_pool,
        1,
        &cmd_buff
    );
}

VkResult vkl_state_frame_begin(vkl_state_t* state) {
    if (state->frame_in_progress) {
        vkl_error(
            "vkl_state_frame_begin: frame was already started!\n",
            ERROR_FATAL
        );
    }

    VkResult result = false;
    result = vkl_state_next_frame(state);
    if (result != VK_SUCCESS) return result;

    state->frame_in_progress = true;

    VkCommandBufferBeginInfo begin_info = { 
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
   
    result = vkBeginCommandBuffer(
        vkl_state_command_buffer(state),
        &begin_info
    );

    if (result != VK_SUCCESS) {
        state->frame_in_progress = false;
        vkl_error(
            "failed to begin recording command buffer!\n",
            ERROR_WARNING
        );
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
        .renderArea.offset  = (VkOffset2D){ 0,0 },
        .renderArea.extent  = state->swapchain->extent,
        .clearValueCount    = 2,
        .pClearValues       = clear_values
    };

    vkCmdBeginRenderPass(
        vkl_state_command_buffer(state),
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
        vkl_state_command_buffer(state),
        0,
        1,
        &viewport
    );

    vkCmdSetScissor(
        vkl_state_command_buffer(state),
        0, 
        1,
        &scissor
    );

    return result;
}

VkResult vkl_state_frame_end(vkl_state_t* state) {
    vkCmdEndRenderPass(
        vkl_state_command_buffer(state)
    );

    if (!state->frame_in_progress) return VK_FALSE;

    VkResult result = vkEndCommandBuffer(
        vkl_state_command_buffer(state)
    );

    if (result != VK_SUCCESS) {
        vkl_error("failed to end command buffer!\n",
            ERROR_WARNING);
        return result;
    }

    result = vkl_state_submit_commands(state);

    state->frame_in_progress = false;
    state->frame_index = (state->frame_index + 1)
        % MAX_FRAMES_IN_FLIGHT;

    return result;
}

void vkl_state_new(vkl_state_info_t* info, vkl_state_t* state) {
    memset(state, 0, sizeof(vkl_state_t));
    state->device = info->bound_device;
    state->swapchain = info->bound_swapchain;
}

void vkl_state_del(vkl_state_t* state) {
    memset(state, 0, sizeof(vkl_state_t));
}