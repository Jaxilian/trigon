#pragma once
#include "trigon/core/std/std.h"
#include "trigon/core/dev/gpu/vk.h"
class window_t;

class renderer_t {
public:
    void destroy();
    VkCommandBuffer cmd_begin();
    void cmd_end(VkCommandBuffer buff);

    VkResult frame_begin();
    VkResult frame_end();

    inline VkCommandBuffer cmdbuff() const {
        return cmdbuffers[frame_index];
    }

private:
    renderer_t();
    ~renderer_t();

    void create_cmdr();
    void destroy_cmdr();

    b8  frame_started = false;
    u32	frame_index = 0;
    u32	image_index = 0;
    u32 cmdbuff_count = 0;

    VkCommandBuffer* cmdbuffers = VK_NULL_HANDLE;

public:
    renderer_t(const renderer_t&) = delete;
    renderer_t& operator=(const renderer_t&) = delete;
    renderer_t(renderer_t&&) = delete;
    renderer_t& operator=(renderer_t&&) = delete;

    static renderer_t& ref() {
        static renderer_t instance;
        return instance;
    }
};