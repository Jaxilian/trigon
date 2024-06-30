#include "vkdef.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "trigon/core/utils/debug.h"
#include "trigon/app.h"
#include "win.h"

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      ms,
    VkDebugUtilsMessageTypeFlagsEXT             mt,
    const VkDebugUtilsMessengerCallbackDataEXT* cbd,
    void* pud) {

    cstr_t err_type;
    switch (mt) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
        err_type = "device address binding";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT:
        err_type = "max enum";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        err_type = "general";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        err_type = "performance";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        err_type = "validation";
        break;
    default:
        err_type = "unkown";
        break;
    }

    if (ms >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        debug_err("type: \"%s\" message: %s\n", err_type, cbd->pMessage);
    }
    else if (ms >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        debug_wrn("type: \"%s\" message: %s\n", err_type, cbd->pMessage);
    }
    else {
        debug_log("type: \"%s\" message: %s\n", err_type, cbd->pMessage);
    }

    return VK_FALSE;
}

static VkResult create_debug_util(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* ci,
    const VkAllocationCallbacks* a,
    VkDebugUtilsMessengerEXT* dm) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, ci, a, dm);
    }
    else {
        debug_err("create_debug_utils_messenger -> extension not present!\n");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void destroy_debug_util(
    VkInstance instance,
    VkDebugUtilsMessengerEXT dm,
    const VkAllocationCallbacks* a) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, dm, a);
    }
}

static void debug_gen_info(VkDebugUtilsMessengerCreateInfoEXT* inf) {
    inf->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    inf->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    inf->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    inf->pfnUserCallback = debug_callback;
    inf->pUserData = NULL;
}

void vkinstance_new(vkdevice_t* device) {

    const char* validations[] = { "VK_LAYER_KHRONOS_validation" };
    #ifdef _DEBUG
        uint32_t validations_count = 1;
    #else
        uint32_t validations_count = 0;
    #endif

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = app_name(),
        .applicationVersion = app_version(),
        .pEngineName = engine_name(),
        .engineVersion = engine_version(),
        .apiVersion = VK_API_VERSION_1_2
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

    uint32_t extc   = 0;
    cstr_t* exts    = win_ext(&extc);

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = extc,
        .ppEnabledExtensionNames = exts,
        .enabledLayerCount = validations_count,
        .ppEnabledLayerNames = validations,
        .pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_info
    };

#ifndef _DEBUG
    create_info.pNext = NULL;
#endif


    if (vkCreateInstance(
        &create_info,
        NULL,
        &device->instance) != VK_SUCCESS) {
        debug_err("failed to find create VkInstance!\n");
    }

#ifdef _DEBUG
    bool success = create_debug_util(device->instance, &debug_info, NULL, &device->debugger) == VK_SUCCESS;
    if (!success) {
        debug_err("failed to create debug messenger!\n");
    }
#endif

    switch (app_info.apiVersion)
    {
    case VK_API_VERSION_1_0:
        debug_log("loaded: vulkan 1.0\n");
        break;
    case VK_API_VERSION_1_1:
        debug_log("loaded: vulkan 1.1\n");
        break;
    case VK_API_VERSION_1_2:
        debug_log("loaded: vulkan 1.2\n");
        break;
    case VK_API_VERSION_1_3:
        debug_log("loaded: vulkan 1.3\n");
        break;
    default:
        break;
    }
   
}

/*---------- DEVICE ----------*/

static void get_queue_families(vkdevice_t* device, bool* success, uint32_t* g_family, uint32_t* p_family) {
    uint32_t    graphics_family = 0;
    uint32_t    present_family = 0;
    bool        graphics_family_valid = false;
    bool        present_family_valid = true;

    uint32_t queue_fam_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queue_fam_count, NULL);

    VkQueueFamilyProperties* queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_fam_count);
    if (queue_families == NULL) {
        debug_err("failed to allocate memory!\n");
    }

    vkGetPhysicalDeviceQueueFamilyProperties(device->physical, &queue_fam_count, queue_families);

    for (uint32_t i = 0; i < queue_fam_count; ++i) {
        if (queue_families[i].queueCount > 0 && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family = i;
            graphics_family_valid = true;
        }

        VkBool32 present_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device->physical, i, device->main_surface, &present_supported);
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

static void destroy_swap_support(vkdevice_t* device) {
    if (device->available_formats) {
        free(device->available_formats);
        device->available_formats_c = 0;
        device->available_formats = NULL;

    }

    if (device->available_present_modes) {
        free(device->available_present_modes);
        device->available_present_modes_c = 0;
        device->available_present_modes = NULL;
    }
}

static void create_swap_support(vkdevice_t* device) {
    destroy_swap_support(device);

    VkSurfaceCapabilitiesKHR capabilities = { 0 };
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physical, device->main_surface, &capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical, device->main_surface, &format_count, NULL);

    if (format_count != 0) {
        device->available_formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
        device->available_formats_c = format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical, device->main_surface, &format_count, device->available_formats);
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->physical, device->main_surface, &present_mode_count, NULL);

    if (present_mode_count != 0) {
        device->available_present_modes = malloc(sizeof(VkPresentModeKHR) * present_mode_count);
        device->available_present_modes_c = present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device->physical,
            device->main_surface,
            &present_mode_count,
            device->available_present_modes);
    }
}

static bool device_ext_valid(vkdevice_t* device) {
    uint32_t extc;
    vkEnumerateDeviceExtensionProperties(device->physical, NULL, &extc, NULL);

    VkExtensionProperties* extensions = malloc(sizeof(VkExtensionProperties) * extc);
    if (extensions == NULL) {
        debug_err("failed to allocate memory for device extensions\n");
    }

    vkEnumerateDeviceExtensionProperties(
        device->physical,
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

static bool device_suitable(vkdevice_t* device) {
    bool valid = false;
    uint32_t graphics_family = 0;
    uint32_t present_family = 0;

    get_queue_families(device, &valid, &graphics_family, &present_family);
    create_swap_support(device);

    bool ext_supported = device_ext_valid(device);

    bool swap_ready = false;
    if (ext_supported) {
        swap_ready = device->available_formats_c > 0
            && device->available_present_modes_c > 0;
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(device->physical, &supported_features);

    device->graphics_family = graphics_family;
    device->present_family = present_family;
    return valid && ext_supported && swap_ready &&
        supported_features.samplerAnisotropy;
}

static void create_physical_device(vkdevice_t* device) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(device->instance, &device_count, NULL);

    if (device_count <= 0) {
        debug_err("failed to find gpu's with vulkan support!\n");
    }

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * device_count);
    if (devices == NULL) {
        debug_err("failed to allocate memory for VkPhysicalDevices format!\n");
    }

    vkEnumeratePhysicalDevices(device->instance, &device_count, devices);

    for (uint32_t i = 0; i < device_count; i++) {
        device->physical = devices[i];
        if (device_suitable(device)) break;
    }

    free(devices);
}

static void vkl_create_device(vkdevice_t* device) {
    
    uint32_t unique_queue_family[2] = { 
        device->graphics_family, 
        device->present_family 
    };
    uint32_t num_unique_families = (unique_queue_family[0] == unique_queue_family[1]) ? 1 : 2;

    VkDeviceQueueCreateInfo queue_create_infos[2];
    float queue_priority = 1.0f;

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
    const char* dev_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = num_unique_families,
        .pQueueCreateInfos = queue_create_infos,
        .pEnabledFeatures = &device_features,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = dev_extensions,
        .enabledLayerCount = 0
    };

    if (vkCreateDevice(device->physical, &create_info, NULL, &device->device) != VK_SUCCESS) {
        debug_err("failed to create VkDevice!\n");
    }

    vkGetDeviceQueue(device->device, unique_queue_family[0], 0, &device->graphics_queue);
    vkGetDeviceQueue(device->device, unique_queue_family[1], 0, &device->present_queue);

    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = unique_queue_family[0],
        .flags =
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    };

    if (vkCreateCommandPool(device->device, &pool_info, NULL, &device->cmd_pool) != VK_SUCCESS) {
        debug_err("failed to create command pool!\n");
    }

    VkCommandBufferAllocateInfo alloc_info = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandPool = device->cmd_pool,
      .commandBufferCount = MAX_FRAMES_IN_FLIGHT

    };

    bool success = false;
    success = vkAllocateCommandBuffers(device->device, &alloc_info, device->cmd_buffers) == VK_SUCCESS;
    if (!success) {
        debug_err("failed to create command buffers!\n");
    }
}

void vkdevice_new(vkdevice_t* device, VkSurfaceKHR surface) {
    device->main_surface = surface;
    create_physical_device(device);
    vkl_create_device(device);
}

void vkdevice_del(vkdevice_t* device) {
    destroy_swap_support(device);

    if (device->cmd_pool) {
        vkFreeCommandBuffers(
            device->device,
            device->cmd_pool,
            MAX_FRAMES_IN_FLIGHT,
            device->cmd_buffers
        );

        vkDestroyCommandPool(device->device, device->cmd_pool, NULL);
    }

    vkDestroyDevice(device->device, NULL);
}

void vkinstance_del(vkdevice_t* device) {
#ifdef _DEBUG
    if (device->debugger != NULL) {
        destroy_debug_util(
            device->instance,
            device->debugger,
            NULL
        );
        device->debugger = NULL;
    }
#endif

    if (device->instance != NULL) {
        vkDestroyInstance(device->instance, NULL);
        device->instance = NULL;
    }
}