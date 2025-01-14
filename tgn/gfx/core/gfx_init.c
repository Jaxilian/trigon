#include "../gfx.h"
#include "gfxctx.h"
#include "cmn/cmn.h"

#ifdef _DEBUG

#ifdef _WIN32
    #define EXTENSIONS {"VK_KHR_surface", "VK_KHR_win32_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME }
    #define EXTENSION_COUNT 3

#else
    #define EXTENSIONS { "VK_KHR_surface", "VK_KHR_wayland_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME }
    #define EXTENSION_COUNT 3
#endif

#else

#ifdef _WIN32
    #define EXTENSIONS {"VK_KHR_surface", "VK_KHR_win32_surface" }
    #define EXTENSION_COUNT 2
#else
    #define EXTENSIONS { "VK_KHR_surface", "VK_KHR_wayland_surface"}
    #define EXTENSION_COUNT 2
#endif

#endif

static VkInstance                  vkinstance = NULL;
static VkDebugUtilsMessengerEXT    vkmessenger = NULL;

VkInstance gfx_vkinstance() { return vkinstance; }

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      ms,
    VkDebugUtilsMessageTypeFlagsEXT             mt,
    const VkDebugUtilsMessengerCallbackDataEXT* cbd,
    void* pud) {

    str_t err_type;
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

void gfx_init(str_t app_name, version_t app_version) {

	VkApplicationInfo appinfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pEngineName = "Trigon",
		.apiVersion = VK_API_VERSION_1_3,
		.pApplicationName = app_name,
		.engineVersion = VK_MAKE_API_VERSION(1,0,0,0),
		.applicationVersion = VERSION_TO_INT(app_version),
		.pNext = NULL,
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

    VkDebugUtilsMessengerCreateInfoEXT* debug_info_ptr = NULL;
    #ifdef _DEBUG
    debug_info_ptr = &debug_info;
    #endif

    str_t validations[] = { "VK_LAYER_KHRONOS_validation" };
    #ifdef _DEBUG
        uint32_t validations_count = 1;
    #else
        uint32_t validations_count = 0;
    #endif

	VkInstanceCreateInfo createinfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appinfo,
		.ppEnabledExtensionNames = (str_t[])EXTENSIONS,
		.enabledExtensionCount = EXTENSION_COUNT,
        .enabledLayerCount = validations_count,
        .ppEnabledLayerNames = validations,
		.pNext = debug_info_ptr
	};
    
    
	vkCreateInstance(&createinfo, NULL, &vkinstance);

#ifdef _DEBUG
    bool success = create_debug_util(vkinstance, &debug_info, NULL, &vkmessenger) == VK_SUCCESS;
    if (!success) {
        debug_err("failed to create debug messenger!\n");
    }
#endif

}


void gfx_quit() {

    if (vkmessenger != NULL) {
        destroy_debug_util(vkinstance, vkmessenger, NULL);
        vkmessenger = NULL;
    }

    if (vkinstance != NULL) {
        vkDestroyInstance(vkinstance, NULL);
        vkinstance = NULL;
    }
}
