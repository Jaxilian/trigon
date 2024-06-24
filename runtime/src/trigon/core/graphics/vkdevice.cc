#include "vkdevice.h"
#include "trigon/core/util/err.h"
#include "trigon/app.h"
#include <SDL3/SDL_vulkan.h>
#include <vector>

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
        debug_t::err("type: \"%s\" message: %s\n", err_type, cbd->pMessage);
    }
    else if (ms >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        debug_t::wrn("type: \"%s\" message: %s\n", err_type, cbd->pMessage);
    }
    else {
        debug_t::log("type: \"%s\" message: %s\n", err_type, cbd->pMessage);
    }

    return VK_FALSE;
}

static VkResult create_debug_util(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* ci,
    const VkAllocationCallbacks* a,
    VkDebugUtilsMessengerEXT* dm) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, ci, a, dm);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


static void destroy_debug_util(
    VkInstance instance,
    VkDebugUtilsMessengerEXT dm,
    const VkAllocationCallbacks* a) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, dm, a);
    }
}

static void debug_gen_info(VkDebugUtilsMessengerCreateInfoEXT& inf) {
    inf = {};
    inf.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    inf.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    inf.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    inf.pfnUserCallback = debug_callback;
    inf.pUserData = nullptr;
}



vkdevice_t::vkdevice_t() {
    const char* validations[] = { "VK_LAYER_KHRONOS_validation" };
#ifdef _DEBUG
    uint32_t validations_count = 1;
#else
    uint32_t validations_count = 0;
#endif


    app_t::config.version.compile();
    app_t::engine_version.compile();

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app_t::config.app_name;
    app_info.applicationVersion = app_t::config.version.merged;
    app_info.pEngineName = "Trigon";
    app_info.engineVersion = app_t::engine_version.merged;
    app_info.apiVersion = VK_API_VERSION_1_3;


    Uint32 extension_count = 0;
    auto extensions = SDL_Vulkan_GetInstanceExtensions(&extension_count);
    std::vector<const char*> enabled_extensions(extensions, extensions + extension_count);

    #ifdef _DEBUG
        enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

    VkInstanceCreateInfo instance_info = {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount = (uint32_t)enabled_extensions.size();
    instance_info.ppEnabledExtensionNames = enabled_extensions.data();
    instance_info.enabledLayerCount = validations_count;
    instance_info.ppEnabledLayerNames = validations;

#ifdef _DEBUG
    VkDebugUtilsMessengerCreateInfoEXT debug_inf = {};
    debug_gen_info(debug_inf);
    instance_info.pNext = &debug_inf;
#endif

    VkInstance inst;
    if (vkCreateInstance(&instance_info, nullptr, &inst) != VK_SUCCESS) {
        debug_t::err("failed to create vulkan instance\n");
    }

    instance = inst;


#ifdef _DEBUG
    VkDebugUtilsMessengerEXT _d;
    if (create_debug_util(inst, &debug_inf, nullptr, &_d) != VK_SUCCESS) {
        debug_t::err("failed to set up debug messenger!\n");
    }
    debugger = _d;
#endif
}

vkdevice_t::~vkdevice_t() {
    if (cmd_pool != nullptr) {
        vkDestroyCommandPool((VkDevice)device, (VkCommandPool)cmd_pool, NULL);
        cmd_pool = nullptr;
    }

    if (device != nullptr) {
        vkDestroyDevice((VkDevice)device, NULL);
        device = nullptr;
    }

#ifdef _DEBUG
    if (debugger != nullptr) {
        destroy_debug_util(
            (VkInstance)instance,
            (VkDebugUtilsMessengerEXT)debugger,
            NULL
        );
    }
#endif

    if (instance != nullptr) {
        vkDestroyInstance((VkInstance)instance, NULL);
        instance = nullptr;
    }
}