#include "gpu.h"
#include <vulkan/vulkan.h>
#include "trigon/core/util/err.h"
#include "trigon/app.h"
#include "win.h"
#include <vector>
#include "vk_swap_supp.h"
#include <set>


static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      ms,
    VkDebugUtilsMessageTypeFlagsEXT             mt,
    const VkDebugUtilsMessengerCallbackDataEXT* cbd,
    void* pud) {
    
    cstr_t err_type;
    switch (mt){
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
    else if(ms >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        debug_t::wrn("type: \"%s\" message: %s\n", err_type, cbd->pMessage);
    }
    else {
        debug_t::log("type: \"%s\" message: %s\n", err_type, cbd->pMessage);
    }
    
    return VK_FALSE;
}

VkResult create_debug_util(
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


void destroy_debug_util(
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

gpu_t::gpu_t() {
    create_instance();
}

gpu_t::~gpu_t() {

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


void gpu_t::create_instance() {

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

    VkInstanceCreateInfo instance_info = {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount = static_cast<uint32_t>(win_t::extension_count);
    instance_info.ppEnabledExtensionNames = win_t::extensions;
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

void gpu_t::get_queue_families(
    void*       physical_device,
    bool&       success,
    uint32_t&   g_family,
    uint32_t&   p_family,
    void*       surface) {
    VkPhysicalDevice gpu = (VkPhysicalDevice)physical_device;

    uint32_t    graphics_family = 0;
    uint32_t    present_family = 0;
    bool        graphics_family_valid = false;
    bool        present_family_valid = true;

    uint32_t queue_fam_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_fam_count, NULL);
    std::vector<VkQueueFamilyProperties> queue_families(queue_fam_count);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queue_fam_count, queue_families.data());

    for (uint32_t i = 0; i < queue_fam_count; ++i) {
        if (queue_families[i].queueCount > 0 && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphics_family = i;
            graphics_family_valid = true;
        }

        VkBool32 present_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, (VkSurfaceKHR)surface, &present_supported);
        if (queue_families[i].queueCount > 0 && present_supported) {
            present_family = i;
            present_family_valid = true;
        }
        if (present_family_valid && graphics_family_valid) {
            break;
        }
    }

    success = present_family_valid && graphics_family_valid;
    p_family = present_family;
    g_family = graphics_family;
}

bool gpu_t::device_suitable(void* physical_device, void* surface) {

    VkPhysicalDevice gpu = (VkPhysicalDevice)physical_device;

    bool        valid = false;
    uint32_t    graphics_family = 0;
    uint32_t    present_family = 0;

    get_queue_families(gpu, valid, graphics_family, present_family, surface);

    bool ext_supported = false;
    {
        uint32_t extc;
        vkEnumerateDeviceExtensionProperties(gpu, NULL, &extc, NULL);
        std::vector< VkExtensionProperties> extensions(extc);

        vkEnumerateDeviceExtensionProperties(
            gpu,
            NULL,
            &extc,
            extensions.data()
        );

        for (uint32_t i = 0; i < extc; i++) {
            if (strcmp(extensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
                ext_supported = true;
                break;
            }
        }
    }
   
    swap_support_t support(gpu, (VkSurfaceKHR)surface);
    bool swap_ready = false;
    if (ext_supported) {
        swap_ready = support.formats.size() > 0
            && support.present_modes.size() > 0;
    }

    VkPhysicalDeviceFeatures supported_features;
    vkGetPhysicalDeviceFeatures(gpu, &supported_features);

    que_fam.graphics_family = graphics_family;
    que_fam.present_family = present_family;
    return valid && ext_supported && swap_ready &&
        supported_features.samplerAnisotropy;
}

void gpu_t::get_physical_device(void* surface) {
    VkInstance inst = (VkInstance)instance;

    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(inst, &device_count, NULL);
    std::vector<VkPhysicalDevice> devs(device_count);

    vkEnumeratePhysicalDevices(inst, &device_count, devs.data());

    for (uint32_t i = 0; i < device_count; i++) {
        if (device_suitable(devs[i], surface)) {
            physical = devs[i];
            break;
        }
    }
}

void gpu_t::bind_device(void* surface) {
    if (device != nullptr) return;
    get_physical_device(surface);

    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    std::set<uint32_t> unique_families = { que_fam.graphics_family, que_fam.present_family };

    float queue_prio = 1.0f;
    for (uint32_t queue_fam : unique_families) {
        VkDeviceQueueCreateInfo queue_info = {};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = queue_fam;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &queue_prio;
        queue_infos.push_back(queue_info);
    }

    VkPhysicalDeviceFeatures features = {};
    features.samplerAnisotropy = VK_TRUE;

    const char* dev_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size());
    create_info.pQueueCreateInfos = queue_infos.data();
    create_info.pEnabledFeatures = &features;
    create_info.enabledExtensionCount =  1;
    create_info.ppEnabledExtensionNames = dev_extensions;

    VkDevice dev = VK_NULL_HANDLE;
    if (vkCreateDevice((VkPhysicalDevice)physical, &create_info, nullptr, &dev) != VK_SUCCESS) {
        debug_t::err("failed to create logical device!\n");
    }

    device = dev;
    VkQueue pque;
    VkQueue gque;
    vkGetDeviceQueue(dev, que_fam.graphics_family, 0, &gque);
    vkGetDeviceQueue(dev, que_fam.present_family, 0, &pque);
    graphics_queue = gque;
    present_queue = pque;

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = que_fam.graphics_family;
    pool_info.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkCommandPool _pool;
    if (vkCreateCommandPool(dev, &pool_info, nullptr, &_pool) != VK_SUCCESS) {
        debug_t::err("failed to create command pool!");
    }

    cmd_pool = _pool;

}