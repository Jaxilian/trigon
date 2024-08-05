#pragma once
#include "trigon/core/std/std.h"
#include "trigon/core/dev/vk/vulkan.h"

class vkdev_t;
class window_t;

class renderer_t {
public:
   
    vkdev_t& get_device() {
        return device;
    };

    swap_t& get_swap() {
        return swap;
    }

    vkcmdr_t& get_cmdr() {
        return cmdr;
    }

    static renderer_t& ref() {
        cassert(instance, "renderer_t::instance was nullptr\n");
        return *instance;
    }

    renderer_t(cstr_t app_name, version_t version);
    ~renderer_t();

private:
  
    vkdev_t     device;
    swap_t      swap;
    vkcmdr_t    cmdr;

    static renderer_t* instance;

public:
    renderer_t(const renderer_t&) = delete;
    renderer_t& operator=(const renderer_t&) = delete;
    renderer_t(renderer_t&&) = delete;
    renderer_t& operator=(renderer_t&&) = delete;   
};