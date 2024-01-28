#include "vkl.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>

static void destroy_debug_utils(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger,
    const VkAllocationCallbacks* alloc) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debug_messenger, alloc);
    }
}

static void vkl_get_queue_families(VkPhysicalDevice gpu, bool* success, uint32_t* g_family, uint32_t* p_family, vkl_device_t* device) {
    uint32_t    graphics_family = 0;
    uint32_t    present_family = 0;
    bool        graphics_family_valid = false;
    bool        present_family_valid = true;

    uint32_t queue_fam_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_fam_count, NULL);

    VkQueueFamilyProperties* queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_fam_count);
    if (queue_families == NULL) {
        vkl_error("failed to allocate memory!\n", ERROR_FATAL);
    }

    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_fam_count, queue_families);

    for (uint32_t i = 0; i < queue_fam_count; ++i) {
        if (queue_families[i].queueCount > 0 && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family = i;
            graphics_family_valid = true;
        }

        VkBool32 present_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, device->surface, &present_supported);
        if (queue_families[i].queueCount > 0 && present_supported) {
            present_family = i;
            present_family_valid = true;
        }
        if (present_family_valid && graphics_family_valid) {
            break;
        }
    }

    free(queue_families);

    *success = present_family_valid && graphics_family_valid;
    *p_family = present_family;
    *g_family = graphics_family;
}

static void vkl_destroy_swap_support(vkl_device_t* dev) {
    free(dev->swap_support.present_modes);
    free(dev->swap_support.formats);
    memset(&dev->swap_support, 0, sizeof(vkl_swap_support_t));
}

static void vkl_create_swap_support(VkPhysicalDevice gpu, vkl_device_t* dev) {
    if (dev->swap_support.formats != NULL || dev->swap_support.present_modes != NULL) {
        vkl_destroy_swap_support(dev);
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, dev->surface, &dev->swap_support.capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, dev->surface, &format_count, NULL);

    if (format_count != 0) {
        dev->swap_support.formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
        dev->swap_support.format_count = format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, dev->surface, &format_count, dev->swap_support.formats);
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, dev->surface, &present_mode_count, NULL);

    if (present_mode_count != 0) {
        dev->swap_support.present_modes = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
        dev->swap_support.present_count = present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            gpu,
            dev->surface,
            &present_mode_count,
            dev->swap_support.present_modes);
    }
}

static bool vkl_device_ext_valid(VkPhysicalDevice gpu) {
    uint32_t extc;
    vkEnumerateDeviceExtensionProperties(gpu, NULL, &extc, NULL);

    VkExtensionProperties* extensions = malloc(sizeof(VkExtensionProperties) * extc);
    if (extensions == NULL) {
        vkl_error("failed to allocate memory for device extensions\n", ERROR_FATAL);
    }

    vkEnumerateDeviceExtensionProperties(
        gpu,
        NULL,
        &extc,
        extensions
    );

    for (uint32_t i = 0; i < extc; i++) {
        if (strcmp(extensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            free(extensions);
            return true;
        }
    }
    free(extensions);
    return false;
}

static bool device_suitable(VkPhysicalDevice gpu, vkl_device_t* device) {
    bool valid = false;
    uint32_t graphics_family = 0;
    uint32_t present_family = 0;

    vkl_get_queue_families(gpu, &valid, &graphics_family, &present_family, device);
    
    vkl_create_swap_support(gpu, device);

    bool ext_supported = vkl_device_ext_valid(gpu);

    bool swap_ready = false;
    if (ext_supported) {
        swap_ready = device->swap_support.format_count > 0
            && device->swap_support.present_count > 0;
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(gpu, &supportedFeatures);
    vkl_destroy_swap_support(device);

    device->graphics_family = graphics_family;
    device->present_family = present_family;
    return valid && ext_supported && swap_ready &&
        supportedFeatures.samplerAnisotropy;
}

static void vkl_create_physical_device(vkl_device_t* device) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(device->instance, &device_count, NULL);

    if (device_count <= 0) {
        vkl_error("failed to find gpu's with vulkan support!\n", ERROR_FATAL);
    }

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * device_count);
    if (devices == NULL) {
        vkl_error("failed to allocate memory for VkPhysicalDevices format!\n", ERROR_FATAL);
    }

    vkEnumeratePhysicalDevices(device->instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++) {
        if (device_suitable(devices[i], device)) {
            device->physical_device = devices[i];
            break;
        }
    }

    free(devices);
}

static void vkl_create_device(const vkl_device_info_t* info, vkl_device_t* device) {
    bool success = false;
    uint32_t gf = 0, pf = 0;
    vkl_get_queue_families(device->physical_device, &success, &gf, &pf, device);

    uint32_t unique_queue_family[2] = { gf, pf };
    uint32_t num_unique_families = (gf == pf) ? 1 : 2;
    device->queue_count = (gf == pf) ? 1 : 2;

    VkDeviceQueueCreateInfo queue_create_infos[2];
    float queue_priority = 1.0f;

    for (uint32_t i = 0; i < num_unique_families; ++i) {
        VkDeviceQueueCreateInfo queue_create_info = { 0 };
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = unique_queue_family[i];
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos[i] = queue_create_info;
        device->queue_indices[i] = unique_queue_family[i];
    }

    VkPhysicalDeviceFeatures device_features = { 0 };
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info = { 
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount       = num_unique_families,
        .pQueueCreateInfos          = queue_create_infos,
        .pEnabledFeatures           = &device_features,
        .enabledExtensionCount      = info->device_extension_count,
        .ppEnabledExtensionNames    = info->device_extensions,
        .enabledLayerCount          = 0
    };

    success = (vkCreateDevice(device->physical_device, &create_info, NULL, &device->device) == VK_SUCCESS);
    if (!success) {
        vkl_error("failed to create VkDevice!\n", ERROR_FATAL);
    }

    vkGetDeviceQueue(device->device, gf, 0, &device->graphics_queue);
    vkGetDeviceQueue(device->device, pf, 0, &device->present_queue);
}

static void vkl_create_command_pool(vkl_device_t* device) {
    VkCommandPoolCreateInfo pool_info = { 
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = device->graphics_family,
        .flags =
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
    };

    bool success = false;
    success = vkCreateCommandPool(device->device, &pool_info, NULL, &device->command_pool) == VK_SUCCESS;
    if (!success) {
        vkl_error("failed to create command pool!\n", ERROR_FATAL);
    }
}

static void vkl_create_command_buffers(vkl_device_t* device) {
    VkCommandBufferAllocateInfo alloc_info = { 
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandPool = device->command_pool,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT
    
    };

    bool success = false;
    success = vkAllocateCommandBuffers(device->device, &alloc_info, device->command_buffers) == VK_SUCCESS;
    if (!success) {
        vkl_error("failed to create command buffers!\n", ERROR_FATAL);
    }
}

void vkl_device_new(const vkl_device_info_t* info, vkl_device_t* device) {
    
    if (device->instance == NULL) {
        vkl_error("you need to create a vkl_instance first!\n", ERROR_FATAL);
    }

    if (device->surface == NULL) {
        vkl_error("vkl_device_t.surface was NULL, create a window surface before calling \"vkl_device_new\"!\n", ERROR_FATAL);
    }

    vkl_create_physical_device(device);
    vkl_create_swap_support(device->physical_device, device);
    vkl_create_device(info, device);
    vkl_create_command_pool(device);
    vkl_create_command_buffers(device);
}

void vkl_device_del(vkl_device_t* device) {
    vkl_destroy_swap_support(device);
    vkFreeCommandBuffers(
        device->device,
        device->command_pool,
        MAX_FRAMES_IN_FLIGHT,
        device->command_buffers
    );
    vkDestroyCommandPool(device->device, device->command_pool, NULL);
    vkDestroyDevice(device->device, NULL);
    vkDestroySurfaceKHR(device->instance, device->surface, NULL);

    if (device->debug_messenger != NULL) {
        destroy_debug_utils(device->instance, device->debug_messenger, NULL);
    }

    vkDestroyInstance(device->instance, NULL);
    memset(device, 0, sizeof(device));
}

void vkl_reset_command_pool(vkl_device_t* device) {
    VkResult res = vkResetCommandPool(device->device, device->command_pool, 0);
    if (res != VK_SUCCESS) {
        vkl_error("failed to reset commnand pool!\n", ERROR_WARNING);
    }
}