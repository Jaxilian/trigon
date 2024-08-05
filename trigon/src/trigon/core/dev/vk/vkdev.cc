#include "vulkan.h"
#include "trigon/core/dev/win/window.h"


/* ------------------------------- DEBUG ------------------------------------*/



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



/* ------------------------------ STATICS -----------------------------------*/



cstr_t			vkdev_t::APP_NAME = "TRIGON_APP";
version_t		vkdev_t::APP_VERSION = version_t(1, 0, 0, 0);
const  cstr_t	vkdev_t::ENGINE_NAME = "Trigon";
const version_t	vkdev_t::ENGINE_VERSION = version_t(1, 0, 0, 0);
vkdev_t*        vkdev_t::this_ptr = nullptr;

/* ------------------------------ CREATION ----------------------------------*/


void vkdev_t::create_instance() {
    this_ptr = this;
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
            &instance
        ) == VK_SUCCESS,
        "failed to find create VkInstance!\n"
    );

#ifdef _DEBUG
    cassert(
        create_debug_util(
            instance,
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

    window_t::create_surface(window_t::main(), surface);
}

void vkdev_t::create_physical() {

    u32 dc = 0;
    vkEnumeratePhysicalDevices(
        instance,
        &dc,
        nullptr
    );

    debug_log("GPU count: %d\n", dc);
    cassert(dc > 0, "failed to find gpu with vulkan support");

    VkPhysicalDevice* gpus = new VkPhysicalDevice[dc];

    vkEnumeratePhysicalDevices(
        instance,
        &dc,
        gpus
    );

    for (u32 i = 0; i < dc; i++) {

        VkPhysicalDevice pd = gpus[i];
        qfi_t qfi(pd);
        swapsupp_t supp(pd);

        b8 ext_supported = extension_supported(
            pd,
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        );

        if (!ext_supported) continue;

        b8 swap_ready = supp.format_count > 0 && supp.present_count > 0;
        b8 qfi_valid = qfi.gfam_valued && qfi.pfam_valued;

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(pd, &features);

        b8 suitable = (
            qfi_valid &&
            swap_ready &&
            ext_supported &&
            features.samplerAnisotropy
            );

        if (!suitable) continue;
        physical = pd;
        pfam = qfi.pfam;
        gfam = qfi.gfam;
        break;
    }

    VkPhysicalDeviceProperties props = {};
    vkGetPhysicalDeviceProperties(physical, &props);
    debug_log("GPU: %s\n", props.deviceName);
    delete[] gpus;

}

void vkdev_t::create_logical() {
    u32 unique_queue_family[2] = {
        gfam,
        pfam
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
        vkCreateDevice(physical, &create_info, NULL, &device) == VK_SUCCESS,
        "failed to create VkDevice!\n"
    );


    vkGetDeviceQueue(device, unique_queue_family[0], 0, &graphics_queue);
    vkGetDeviceQueue(device, unique_queue_family[1], 0, &present_queue);

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = unique_queue_family[0];
    pool_info.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    cassert(
        vkCreateCommandPool(device, &pool_info, NULL, &cmdpool) == VK_SUCCESS,
        "failed to create command pool!\n"
    );

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    alloc_info.commandPool = cmdpool;

    debug_log("Virtual GPU built\n");
}


vkdev_t::vkdev_t(cstr_t app_name, version_t app_ver) {

	APP_NAME	= app_name;
	APP_VERSION = app_ver;

	create_instance();
	create_physical();
	create_logical();
}


/* ----------------------------- DESTRUCTION --------------------------------*/


void vkdev_t::destroy_instance() {
    if (!instance) return;

    vkDestroySurfaceKHR(instance, surface, NULL);

#ifdef _DEBUG
    destroy_debug_util(instance, vkdbg, NULL);
#endif

    vkDestroyInstance(instance, NULL);
    instance = VK_NULL_HANDLE;

}

void vkdev_t::destroy_physical() {

  
}

void vkdev_t::destroy_logical() {
    if (cmdpool) {
        vkDestroyCommandPool(device, cmdpool, NULL);
        cmdpool = VK_NULL_HANDLE;
    }

    if (device) {
        vkDestroyDevice(device, NULL);
        device = VK_NULL_HANDLE;
    }
}


vkdev_t::~vkdev_t() {
	destroy_logical();
	destroy_physical();
	destroy_instance();
}


/* ------------------------------ FUNCTIONS ---------------------------------*/


b8 vkdev_t::extension_supported(VkPhysicalDevice pd, cstr_t name) {
    uint32_t extc;
    vkEnumerateDeviceExtensionProperties(
        pd,
        NULL,
        &extc,
        NULL
    );

    VkExtensionProperties* extensions = new VkExtensionProperties[extc];

    vkEnumerateDeviceExtensionProperties(
        pd,
        NULL,
        &extc,
        extensions
    );

    for (uint32_t i = 0; i < extc; i++) {
        if (strcmp(
            extensions[i].extensionName,
            name) == 0) {
            delete[] extensions;
            return true;
        }
    }
    delete[] extensions;
    return false;
}
u32 vkdev_t::get_mem_type(u32 type, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties props = {};
    vkGetPhysicalDeviceMemoryProperties(physical, &props);

    for (u32 i = 0; i < props.memoryTypeCount; i++) {
        if ((type & (1 << i))
            &&
            (props.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    debug_err("failed to find suitable memory type!");
    return 0;
}

VkFormat vkdev_t::format_supported(
    const VkFormat* candidates,
    u32 candidate_count,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) {

    for (u32 i = 0; i < candidate_count; i++) {
        VkFormatProperties props = {};
        vkGetPhysicalDeviceFormatProperties(physical, candidates[i], &props);
        if (tiling == VK_IMAGE_TILING_LINEAR &&
            (props.linearTilingFeatures & features) == features) {
            return  candidates[i];
        }
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL &&
            (props.optimalTilingFeatures & features) == features) {
            return  candidates[i];
        }
    }

    debug_err("failed to find supported format!\n");
    return (VkFormat)0;

}