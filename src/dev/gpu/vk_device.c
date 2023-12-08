#include "vk.h"
#include "dev/win/win.h"
#include <memory.h>

static bool support_open = false;

#ifdef _DEBUG
#include <string.h>
static VkDebugUtilsMessengerEXT debug_messenger;

static VkBool32 debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
    VkDebugUtilsMessageTypeFlagsEXT             message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data
) {

    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        validate(false, callback_data->pMessage);
    }
    else {
        debug_log("%s\n", callback_data->pMessage);
    }
    return VK_FALSE;
}

static VkResult create_debug_utils(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void destroy_debug_utils(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

static void create_debug_messenger(VkInstance instance) {
    VkDebugUtilsMessengerCreateInfoEXT debug_info = {
       .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
       .messageSeverity =
           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
       .messageType =
           VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
       .pfnUserCallback = debug_callback,
       .pUserData = NULL
    };

    validate(create_debug_utils(instance, &debug_info, NULL, &debug_messenger) == VK_SUCCESS,
        "failed to create debug messenger!\n");
}


#endif

uint32_t vk_fetch_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(ctx->device.gpu, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    validate(false, "failed to find suitable memory type!");
    return 0;
}

void vk_create_image_with_info(
    const VkImageCreateInfo* info,
    VkMemoryPropertyFlags properties,
    VkImage* image,
    VkDeviceMemory* imageMemory)
{
    bool success = vkCreateImage(ctx->device.device, info, NULL, image) == VK_SUCCESS;
    validate(success, "failed to create image!\n");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(ctx->device.device, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = { 0 };
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vk_fetch_memory_type(memRequirements.memoryTypeBits, properties);

    success = vkAllocateMemory(ctx->device.device, &allocInfo, NULL, imageMemory) == VK_SUCCESS;
    validate(success, "failed to allocate image memory!");

    success = vkBindImageMemory(ctx->device.device, *image, *imageMemory, 0) == VK_SUCCESS;
    validate(success, "failed to bind image memory!");
}

VkFormat vk_format_supported(
    const VkFormat* candidates, uint32_t candidate_count, VkImageTiling tiling, VkFormatFeatureFlags features) {

    for (uint32_t i = 0; i < candidate_count; i++) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(ctx->device.gpu, candidates[i], &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return  candidates[i];
        }
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return  candidates[i];
        }
    }

    validate(false, "failed to find supported format!\n");
    return 0;
}

swap_support_t* vk_open_swap_support(VkPhysicalDevice device) {
    validate(!support_open, "swap support cache was already open!\n");
    swap_support_t* support = mem_alloc(sizeof(swap_support_t));
    validate(support, "failed to allocate memory for swap_support_t\n");
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, ctx->win.surface, &support->capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, ctx->win.surface, &format_count, NULL);

    if (format_count != 0) {
        support->formats = mem_alloc(sizeof(VkSurfaceFormatKHR) * format_count);
        support->format_count = format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, ctx->win.surface, &format_count, support->formats);
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, ctx->win.surface, &present_mode_count, NULL);

    if (present_mode_count != 0) {
        support->present_modes = mem_alloc(sizeof(VkPresentModeKHR) * present_mode_count);
        support->present_count = present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            ctx->win.surface,
            &present_mode_count,
            support->present_modes);
    }
    return support;
}

void vk_close_swap_support(swap_support_t* support) {
    mem_free(support->present_modes);
    mem_free(support->formats);
    mem_free(support);
    support_open = false;
}

static void vk_instance_new() {
    uint32_t    extc = 0;
    cstr* ext = win_open_extensions(&extc);

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName   = ctx->app_data.name,
        .applicationVersion = ctx->app_data.version,
        .pEngineName        = "trigon",
        .engineVersion      = VK_MAKE_API_VERSION(1, 0, 0, 0),
        .apiVersion         = VK_API_VERSION_1_3
    };

#ifdef _DEBUG
    uint32_t    layc = 1;
    cstr        lay[1] = {
        "VK_LAYER_KHRONOS_validation"
    };

    VkDebugUtilsMessengerCreateInfoEXT debug_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
        .pUserData = NULL
    };

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo           = &app_info,
        .enabledExtensionCount      = extc,
        .ppEnabledExtensionNames    = ext,
        .enabledLayerCount          = layc,
        .ppEnabledLayerNames        = lay,
        .pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_info
    };
#else
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = extc,
        .ppEnabledExtensionNames = ext,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .pNext = NULL
    };
#endif
    validate(
        vkCreateInstance(
            &create_info,
            NULL,
            &ctx->device.instance
        ) == VK_SUCCESS,
        "failed to create vk instance"
    );

    win_close_extensions();
#ifdef _DEBUG
    create_debug_messenger(ctx->device.instance);
#endif
}

static void vk_get_queue_families(VkPhysicalDevice device, bool* success, uint32_t* g_family, uint32_t* p_family) {
    uint32_t    graphics_family         = 0;
    uint32_t    present_family          = 0;
    bool        graphics_family_valid   = false;
    bool        present_family_valid    = true;

    uint32_t queue_fam_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_fam_count, NULL);

    VkQueueFamilyProperties* queue_families = mem_alloc(sizeof(VkQueueFamilyProperties) * queue_fam_count);
    validate(queue_families, "failed to allocate queue_families!\n");
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_fam_count, queue_families);

    for (uint32_t i = 0; i < queue_fam_count; ++i) {
        if (queue_families[i].queueCount > 0 && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family = i;
            graphics_family_valid = true;
        }

        VkBool32 present_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, ctx->win.surface, &present_supported);
        if (queue_families[i].queueCount > 0 && present_supported) {
            present_family = i;
            present_family_valid = true;
        }
        if (present_family_valid && graphics_family_valid) {
            break;
        }
    }

    mem_free(queue_families);

    *success = present_family_valid && graphics_family_valid;
    *p_family = present_family;
    *g_family = graphics_family;
}

static bool device_ext_valid(VkPhysicalDevice device) {
    uint32_t extc;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extc, NULL);

    VkExtensionProperties* extensions = mem_alloc(sizeof(VkExtensionProperties) * extc);
    validate(extensions, "failed to allocate vulkan extensions!\n");
    vkEnumerateDeviceExtensionProperties(
        device,
        NULL,
        &extc,
        extensions
    );

    for (uint32_t i = 0; i < extc; i++) {
        if (strcmp(extensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            mem_free(extensions);
            return true;
        }
    }
    mem_free(extensions);
    return false;
}

static bool device_suitable(VkPhysicalDevice device) {
    bool valid = false;
    uint32_t graphics_family = 0;
    uint32_t present_family = 0;

    vk_get_queue_families(device, &valid, &graphics_family, &present_family);

    swap_support_t* support = vk_open_swap_support(device);

    bool ext_supported = device_ext_valid(device);

    bool swap_ready = false;
    if (ext_supported) {
        swap_ready = support->format_count > 0
            && support->present_count > 0;
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    vk_close_swap_support(support);

    ctx->device.graphics_family = graphics_family;
    ctx->device.present_family = present_family;
    return valid && ext_supported && swap_ready &&
        supportedFeatures.samplerAnisotropy;
}

static void vk_create_gpu_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(ctx->device.instance, &device_count, NULL);
    validate(device_count > 0, "failed to find GPUs with Vulkan support!");

    VkPhysicalDevice* devices = mem_alloc(sizeof(VkPhysicalDevice) * device_count);
    validate(devices, "failed to allocate physical devices!\n");
    vkEnumeratePhysicalDevices(ctx->device.instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++) {
        if (device_suitable(devices[i])) {
            ctx->device.gpu = devices[i];
            break;
        }
    }

    mem_free(devices);
}

static void vk_create_device() {
    bool success = false;
    uint32_t gf = 0, pf = 0;
    vk_get_queue_families(ctx->device.gpu, &success, &gf, &pf);

    uint32_t unique_queue_family[2] = { gf, pf };
    uint32_t num_unique_families = (gf == pf) ? 1 : 2;

    VkDeviceQueueCreateInfo queue_create_infos[2];
    float queue_priority = 1.0f;

    char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    for (uint32_t i = 0; i < num_unique_families; ++i) {
        VkDeviceQueueCreateInfo queue_create_info = { 0 };
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = unique_queue_family[i];
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos[i] = queue_create_info;
    }

    VkPhysicalDeviceFeatures device_features = { 0 };
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = { 0 };
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = num_unique_families;
    createInfo.pQueueCreateInfos = queue_create_infos;

    createInfo.pEnabledFeatures = &device_features;
    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = device_extensions;
    createInfo.enabledLayerCount = 0;

    success = (vkCreateDevice(ctx->device.gpu, &createInfo, NULL, &ctx->device.device) == VK_SUCCESS);
    validate(success, "failed to create logical device!");

    vkGetDeviceQueue(ctx->device.device, gf, 0, &ctx->device.graphics_queue);
    vkGetDeviceQueue(ctx->device.device, pf, 0, &ctx->device.present_queue);
}

static void create_command_pool() {
    VkCommandPoolCreateInfo pool_info = { 0 };
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = ctx->device.graphics_family;
    pool_info.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    bool success = false;
    success = vkCreateCommandPool(ctx->device.device, &pool_info, NULL, &ctx->device.command_pool) == VK_SUCCESS;
    validate(success, "failed to create command pool!");
}

static void create_command_buffers() {
    VkCommandBufferAllocateInfo alloc_info = { 0 };
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = ctx->device.command_pool;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    bool success = false;
    success = vkAllocateCommandBuffers(ctx->device.device, &alloc_info, ctx->device.command_buffers) == VK_SUCCESS;
    validate(success, "failed to allocate command buffers!");
}

void vk_device_new() {
    vk_instance_new();
    glfwCreateWindowSurface(ctx->device.instance, ctx->win.handle, NULL, &ctx->win.surface);
    vk_create_gpu_device();
    vk_create_device();
    create_command_pool();
    create_command_buffers();
    vkGetPhysicalDeviceProperties(ctx->device.gpu, &ctx->device.gpu_properties);
}

void vk_device_del() {
    vkFreeCommandBuffers(
        ctx->device.device,
        ctx->device.command_pool,
        MAX_FRAMES_IN_FLIGHT,
        ctx->device.command_buffers
    );
    vkDestroyCommandPool(ctx->device.device, ctx->device.command_pool, NULL);
    vkDestroyDevice(ctx->device.device, NULL);
    vkDestroySurfaceKHR(ctx->device.instance, ctx->win.surface, NULL);

#ifdef _DEBUG
    destroy_debug_utils(ctx->device.instance, debug_messenger, NULL);
#endif

    vkDestroyInstance(ctx->device.instance, NULL);
    memset(&ctx->device, 0, sizeof(vk_device_t));
}