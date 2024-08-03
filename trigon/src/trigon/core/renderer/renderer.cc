#include "renderer.h"

cstr_t    renderer_t::APP_NAME		= "TRIGON_APP";
version_t renderer_t::APP_VERSION	= version_t(1, 0, 0, 0);

#ifdef _DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      ms,
    VkDebugUtilsMessageTypeFlagsEXT             mt,
    const VkDebugUtilsMessengerCallbackDataEXT* cbd,
    vptr_t pud) {

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
        cassert(false, "type: \"%s\" message: %s\n", err_type, cbd->pMessage);
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

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(
            instance,
            "vkCreateDebugUtilsMessengerEXT"
        );

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
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT"
        );
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
#endif

void renderer_t::init() {

    cstr_t validations[] = {
        "VK_LAYER_KHRONOS_validation"
    };
#ifdef _DEBUG
    u32 validations_count = 1;
#else
    u32 validations_count = 0;
#endif

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO
    };
    app_info.pApplicationName = APP_NAME;
    app_info.applicationVersion = APP_VERSION.combined();
    app_info.pEngineName = ENGINE_NAME;
    app_info.engineVersion = ENGINE_VERSION.combined();
    app_info.apiVersion = VK_API_VERSION_1_2;

#ifdef _DEBUG
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
    vptr_t debug_info_ptr = &debug_info;
#else
    vptr_t debug_info_ptr = NULL;
#endif

    u32 extc = 0;
    cstr_t* exts = window_t::extensions(&extc);

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
    };
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = extc;
    create_info.ppEnabledExtensionNames = exts;
    create_info.enabledLayerCount = validations_count;
    create_info.ppEnabledLayerNames = validations;
    create_info.pNext = debug_info_ptr;

    cassert(
        vkCreateInstance(
            &create_info,
            NULL,
            &vkinst
        ) == VK_SUCCESS,
        "failed to find create VkInstance!\n"
    );

#ifdef _DEBUG
    cassert(
        create_debug_util(
            vkinst,
            &debug_info,
            NULL,
            &vkdbg
        ) == VK_SUCCESS,
        "failed to find create vkdbg!\n"
    );
#endif

    switch (app_info.apiVersion)
    {
    case VK_API_VERSION_1_0:
        debug_log("Vulkan: 1.0\n");
        break;
    case VK_API_VERSION_1_1:
        debug_log("Vulkan: 1.1\n");
        break;
    case VK_API_VERSION_1_2:
        debug_log("Vulkan: 1.2\n");
        break;
    case VK_API_VERSION_1_3:
        debug_log("Vulkan: 1.3\n");
        break;
    default:
        break;
    }

    initialized = true;
}

