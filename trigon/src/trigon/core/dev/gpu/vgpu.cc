#include "gpu.h"

vgpu_t::vgpu_t() {

    gpu_t& gpu = gpu_t::ref();
    u32 unique_queue_family[2] = {
        gpu.gfam,
        gpu.pfam
    };

    u32 num_unique = (unique_queue_family[0] == unique_queue_family[1]) ? 1 : 2;

    VkDeviceQueueCreateInfo queue_create_infos[2];
    float queue_priority = 1.0f;

    for (uint32_t i = 0; i < num_unique; ++i) {
        VkDeviceQueueCreateInfo queue_create_info = { };
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = unique_queue_family[i];
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos[i] = queue_create_info;
    }

    VkPhysicalDeviceFeatures device_features = { };
    device_features.samplerAnisotropy = VK_TRUE;
    cstr_t dev_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = num_unique,
        .pQueueCreateInfos = queue_create_infos,
        .enabledLayerCount = 0,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = dev_extensions,
        .pEnabledFeatures = &device_features,
    };

    cassert(
        vkCreateDevice(gpu.handle, &create_info, NULL, &handle) == VK_SUCCESS,
        "failed to create VkDevice!\n"
    );


    vkGetDeviceQueue(handle, unique_queue_family[0], 0, &graphics_queue);
    vkGetDeviceQueue(handle, unique_queue_family[1], 0, &present_queue);

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = unique_queue_family[0];
    pool_info.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
   
    cassert(
        vkCreateCommandPool(handle, &pool_info, NULL, &cmdpool) == VK_SUCCESS,
        "failed to create command pool!\n"
    );

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    alloc_info.commandPool = cmdpool;

    debug_log("Virtual GPU built\n");
}

void vgpu_t::destroy() {
    if (cmdpool) {
        vkDestroyCommandPool(handle, cmdpool, NULL);
        cmdpool = VK_NULL_HANDLE;
    }
    if (handle) {
        vkDestroyDevice(handle, NULL);
        handle = VK_NULL_HANDLE;
    }
}

vgpu_t::~vgpu_t() {
    destroy();
}