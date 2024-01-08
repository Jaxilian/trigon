#include "vkl.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>

static VkBool32 debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
    VkDebugUtilsMessageTypeFlagsEXT             message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data
) {

    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        vkl_error(callback_data->pMessage, ERROR_FATAL);
    }
    else {
        vkl_error(callback_data->pMessage, ERROR_WARNING);
    }
    return VK_FALSE;
}


static VkResult create_debug_utils(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* create_info,
    const VkAllocationCallbacks* alloc,
    VkDebugUtilsMessengerEXT* debug_messenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, create_info, alloc, debug_messenger);
    }
    else {
        vkl_error("create_debug_utils_messenger -> extension not present!\n", ERROR_FATAL);
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


void vkl_instance_new(const vkl_instance_info_t* info, vkl_device_t* device) {
    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = info->app_name,
        .applicationVersion = info->app_version,
        .pEngineName = info->eng_name,
        .engineVersion = info->eng_version,
        .apiVersion = info->vk_version
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
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo           = &app_info,
        .enabledExtensionCount      = info->instance_ext_count,
        .ppEnabledExtensionNames    = info->instance_ext,
        .enabledLayerCount          = info->opt_validation_layer_count,
        .ppEnabledLayerNames        = info->opt_validation_layers,
        .pNext                      = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_info
    };


    if (vkCreateInstance(
        &create_info,
        NULL,
        &device->instance) != VK_SUCCESS) {
        vkl_error("failed to find create VkInstance!\n", ERROR_FATAL);
    }

    if (info->opt_validation_layers != NULL) {
        bool success = create_debug_utils(device->instance, &debug_info, NULL, &device->debug_messenger) == VK_SUCCESS;
        if (!success) {
            vkl_error("failed to create debug messenger!\n", ERROR_FATAL);
        }
    }
}