#include "vk.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static vkctx_t ctx = { 0 };

static cstr_t ENGINE_NAME               = "Trigon";
static const version_t ENGINE_VERSION   = { 1,0,0,0 };

#if defined(_DEBUG) && defined(_WIN32)
#define EXTENSION_COUNT 3
static cstr_t WIN_EXTENSIONS[EXTENSION_COUNT] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface",
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};

#elif defined(_NDEBUG) && defined(_WIN32)
#define EXTENSION_COUNT 2
static cstr_t WIN_EXTENSIONS[EXTENSION_COUNT] = {
    "VK_KHR_surface",
    "VK_KHR_win32_surface"
};

#endif


/*---------------------------------- DEBUG ----------------------------------*/


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


/*-------------------------------- INSTANCE ---------------------------------*/


static void vk_instance(cstr_t app_name, version_t app_version) {
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
    app_info.pApplicationName = app_name;
    app_info.applicationVersion = version_to_int(app_version);
    app_info.pEngineName = "Trigon";
    app_info.engineVersion = VK_MAKE_API_VERSION(1,0,0,0);
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

    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
    };
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = EXTENSION_COUNT;
    create_info.ppEnabledExtensionNames = WIN_EXTENSIONS;
    create_info.enabledLayerCount = validations_count;
    create_info.ppEnabledLayerNames = validations;
    create_info.pNext = debug_info_ptr;

    cassert(
        vkCreateInstance(
            &create_info,
            NULL,
            &ctx.dev.instance
        ) == VK_SUCCESS,
        "failed to find create VkInstance!\n"
    );

#ifdef _DEBUG
    cassert(
        create_debug_util(
            ctx.dev.instance,
            &debug_info,
            NULL,
            &ctx.dev.debugger
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

    char ver_buff[64] = { 0 };

    version_to_string(ver_buff, &ENGINE_VERSION);
    debug_log("%s: %s\n", ENGINE_NAME, ver_buff);

    version_to_string(ver_buff, &app_version);
    debug_log("%s: %s\n", app_name, ver_buff);
}

/*--------------------------------- DEVICE ----------------------------------*/

static b8 get_qfi() {
    u32 qfic = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(ctx.dev.physical, &qfic, NULL);

    VkQueueFamilyProperties* qf = (VkQueueFamilyProperties*)malloc(
        sizeof(VkQueueFamilyProperties) * qfic
    );

    cassert(qf, "qfi_t failed to allocate memory!\n");

    vkGetPhysicalDeviceQueueFamilyProperties(
        ctx.dev.physical,
        &qfic,
        qf
    );

    for (u32 i = 0; i < qfic; ++i) {
        if (qf[i].queueCount > 0 && qf[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            ctx.dev.qfi.gfam = i;
            ctx.dev.qfi.gfam_valued = true;
        }

        VkBool32 present_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            ctx.dev.physical,
            i,
            ctx.dev.surface,
            &present_supported
        );

        if (qf[i].queueCount > 0 && present_supported) {
            ctx.dev.qfi.pfam = i;
            ctx.dev.qfi.pfam_valued = true;
        }
        if (ctx.dev.qfi.gfam_valued && ctx.dev.qfi.pfam_valued) {
            free(qf);
            return true;
        }
    }

    free(qf);
    return false;
}

static void get_ssd() {
    if (ctx.dev.ssd.presents) {
        free(ctx.dev.ssd.presents);
        ctx.dev.ssd.presents = VK_NULL_HANDLE;
    }

    if (ctx.dev.ssd.formats) {
        free(ctx.dev.ssd.formats);
        ctx.dev.ssd.formats = VK_NULL_HANDLE;
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        ctx.dev.physical,
        ctx.dev.surface,
        &ctx.dev.ssd.capabilities
    );

    vkGetPhysicalDeviceSurfaceFormatsKHR(
        ctx.dev.physical,
        ctx.dev.surface,
        &ctx.dev.ssd.format_count,
        NULL
    );

    if (ctx.dev.ssd.format_count != 0) {
       
        ctx.dev.ssd.formats = (VkSurfaceFormatKHR*)malloc( 
            sizeof(VkSurfaceFormatKHR) * ctx.dev.ssd.format_count
        );


        vkGetPhysicalDeviceSurfaceFormatsKHR(
            ctx.dev.physical,
            ctx.dev.surface,
            &ctx.dev.ssd.format_count,
            ctx.dev.ssd.formats
        );
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(
        ctx.dev.physical,
        ctx.dev.surface,
        &ctx.dev.ssd.present_count,
        NULL
    );

    if (ctx.dev.ssd.present_count != 0) {

        ctx.dev.ssd.presents = (VkPresentModeKHR*)malloc(
            sizeof(VkPresentModeKHR) * ctx.dev.ssd.present_count
        );

        vkGetPhysicalDeviceSurfacePresentModesKHR(
            ctx.dev.physical,
            ctx.dev.surface,
            &ctx.dev.ssd.present_count,
            ctx.dev.ssd.presents
        );
    }
}

static b8 ext_supported(cstr_t name) {
    uint32_t extc;
    vkEnumerateDeviceExtensionProperties(
        ctx.dev.physical,
        NULL,
        &extc,
        NULL
    );

    VkExtensionProperties* extensions = (VkExtensionProperties*)malloc(
        sizeof(VkExtensionProperties) * extc
    );
    cassert(extensions, "memory allocation error!\n");

    vkEnumerateDeviceExtensionProperties(
        ctx.dev.physical,
        NULL,
        &extc,
        extensions
    );

    for (uint32_t i = 0; i < extc; i++) {
        if (strcmp(
            extensions[i].extensionName,
            name) == 0) {
            free(extensions);
            return true;
        }
    }
    free(extensions);
    return false;
}

static void vk_device() {
    u32 dc = 0;
    vkEnumeratePhysicalDevices(
        ctx.dev.instance,
        &dc,
        NULL
    );

    debug_log("GPU count: %d\n", dc);
    cassert(dc > 0, "failed to find gpu with vulkan support");

    VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(
        sizeof(VkPhysicalDevice) * dc
    );

    vkEnumeratePhysicalDevices(
        ctx.dev.instance,
        &dc,
        gpus
    );

    for (u32 i = 0; i < dc; i++) {
        ctx.dev.physical = gpus[i];
        get_qfi();
        get_ssd();

        b8 supported = ext_supported(
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        );

        if (!supported) continue;

        b8 swap_ready = ctx.dev.ssd.format_count > 0 &&
            ctx.dev.ssd.present_count > 0;

        b8 qfi_valid = ctx.dev.qfi.gfam_valued && ctx.dev.qfi.pfam_valued;

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(ctx.dev.physical, &features);

        b8 suitable = (
            qfi_valid &&
            swap_ready &&
            ext_supported &&
            features.samplerAnisotropy
            );

        if (!suitable) continue;

        break;
    }

    VkPhysicalDeviceProperties props = {0};
    vkGetPhysicalDeviceProperties(ctx.dev.physical, &props);
    debug_log("GPU: %s\n", props.deviceName);
    free(gpus);

    // LOGICAL DEVICE ---------------

    u32 unique_queue_family[2] = {
        ctx.dev.qfi.gfam,
        ctx.dev.qfi.pfam
    };

    u32 num_unique = (unique_queue_family[0] == unique_queue_family[1]) ? 1 : 2;

    VkDeviceQueueCreateInfo queue_create_infos[2];
    float queue_priority = 1.0f;

    for (uint32_t i = 0; i < num_unique; ++i) {
        VkDeviceQueueCreateInfo queue_create_info = {0};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = unique_queue_family[i];
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos[i] = queue_create_info;
    }

    VkPhysicalDeviceFeatures device_features = {0};
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
        vkCreateDevice(
            ctx.dev.physical,
            &create_info,
            NULL,
            &ctx.dev.device) == VK_SUCCESS,
        "failed to create VkDevice!\n"
    );


    vkGetDeviceQueue(
        ctx.dev.device, unique_queue_family[0], 0, &ctx.dev.graphics_queue);
    vkGetDeviceQueue(
        ctx.dev.device, unique_queue_family[1], 0, &ctx.dev.present_queue);

    VkCommandPoolCreateInfo pool_info = {0};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = unique_queue_family[0];
    pool_info.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    cassert(
        vkCreateCommandPool(
            ctx.dev.device, &pool_info, NULL, &ctx.dev.cmd_pool) == VK_SUCCESS,
        "failed to create command pool!\n"
    );

    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    alloc_info.commandPool = ctx.dev.cmd_pool;

    debug_log("Virtual GPU built\n");

    VkCommandBufferAllocateInfo info = {0};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = ctx.dev.cmd_pool;
    info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    cassert(
        vkAllocateCommandBuffers(
            ctx.dev.device,
            &info,
            ctx.dev.cmd_buffers
        ) == VK_SUCCESS,
        "renderer_t: failed to allocate command buffers!\n"
    );
}

static u32 get_mem_type(u32 type, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties props = {0};
    vkGetPhysicalDeviceMemoryProperties(ctx.dev.physical, &props);

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

static VkFormat format_supported(
    const VkFormat* candidates,
    u32 candidate_count,
    VkImageTiling tiling,
    VkFormatFeatureFlags features) {

    for (u32 i = 0; i < candidate_count; i++) {
        VkFormatProperties props = {0};
        vkGetPhysicalDeviceFormatProperties(
            ctx.dev.physical,
            candidates[i],
            &props
        );

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

/*------------------------------- SWAPCHAIN ---------------------------------*/

static VkPresentModeKHR get_present_mode(
    VkPresentModeKHR* available, u32 count) {

    for (u32 i = 0; i < count; i++) {
        if (available[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available[i];
        }
    }
    /*
    for (u32 i = 0; i < count; i++) {
        if (available[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            debug_log("Use: Immediate\n");
            return available[i];
        }
    }
    */
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D get_extent() {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        ctx.dev.physical,
        ctx.dev.surface,
        &capabilities
    );

    VkExtent2D extent = {0};
    extent.width = ctx.win.extent[0];
    extent.height = ctx.win.extent[1];

    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    }
    else {
        extent.width =
            (extent.width < capabilities.minImageExtent.width) ?
            capabilities.minImageExtent.width :
            (extent.width > capabilities.maxImageExtent.width) ?
            capabilities.maxImageExtent.width : extent.width;

        extent.height = (extent.height < capabilities.minImageExtent.height) ?
            capabilities.minImageExtent.height :
            (extent.height > capabilities.maxImageExtent.height) ?
            capabilities.maxImageExtent.height : extent.height;
        return extent;
    }
}

static void vk_swapchain() {
    u32 old_image_count = ctx.swap.image_count;

    // Recreate swapchain
    {
        ctx.swap.old_swap = ctx.swap.new_swap;

        VkSurfaceFormatKHR surface_format = { 0 };
        b8 found = false;
        for (u32 i = 0; i < ctx.dev.ssd.format_count; i++) {
            if (ctx.dev.ssd.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
                ctx.dev.ssd.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surface_format = ctx.dev.ssd.formats[i];
                found = true;
                break;
            }
        }
        if (!found) {
            debug_wrn("didn't find swap format, using index [0]\n");
            surface_format = ctx.dev.ssd.formats[0];
        }

        ctx.swap.image_format = surface_format.format;

        VkPresentModeKHR present_mode = get_present_mode(
            ctx.dev.ssd.presents,
            ctx.dev.ssd.present_count
        );

        ctx.swap.extent = get_extent();

        ctx.swap.image_count = ctx.dev.ssd.capabilities.minImageCount + 1;
        if (ctx.dev.ssd.capabilities.maxImageCount > 0 &&
            ctx.swap.image_count > ctx.dev.ssd.capabilities.maxImageCount) {
            ctx.swap.image_count = ctx.dev.ssd.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR info = { 0 };
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.surface = ctx.dev.surface;
        info.minImageCount = ctx.swap.image_count;
        info.imageFormat = surface_format.format;
        info.imageColorSpace = surface_format.colorSpace;
        info.imageExtent = ctx.swap.extent;
        info.imageArrayLayers = 1;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        u32 qfis[] = { ctx.dev.qfi.gfam, ctx.dev.qfi.pfam };

        if (ctx.dev.qfi.gfam != ctx.dev.qfi.pfam) {
            info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = qfis;
        }
        else {
            info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.queueFamilyIndexCount = 0; // Optional
            info.pQueueFamilyIndices = NULL; // Optional
        }

        info.preTransform = ctx.dev.ssd.capabilities.currentTransform;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = present_mode;
        info.clipped = VK_TRUE;
        info.oldSwapchain = ctx.swap.old_swap == NULL ? VK_NULL_HANDLE : ctx.swap.old_swap;

        cassert(
            vkCreateSwapchainKHR(
                ctx.dev.device,
                &info,
                NULL,
                &ctx.swap.new_swap
            ) == VK_SUCCESS,
            "swap_t failed to create swapchain!\n"
        );

        if (ctx.swap.old_swap) {
            vkDestroySwapchainKHR(ctx.dev.device, ctx.swap.old_swap, NULL);
            ctx.swap.old_swap = VK_NULL_HANDLE;
        }

        vkGetSwapchainImagesKHR(
            ctx.dev.device,
            ctx.swap.new_swap,
            &ctx.swap.image_count,
            NULL
        );

        VkImage* new_images = realloc(ctx.swap.images, sizeof(VkImage) * ctx.swap.image_count);
        cassert(new_images, "failed to reallocate memory for swap images!");
        ctx.swap.images = new_images;

        vkGetSwapchainImagesKHR(
            ctx.dev.device,
            ctx.swap.new_swap,
            &ctx.swap.image_count,
            ctx.swap.images
        );
    }

    // Create render pass
    {
        if (ctx.swap.renderpass) {
            vkDestroyRenderPass(ctx.dev.device, ctx.swap.renderpass, NULL);
            ctx.swap.renderpass = VK_NULL_HANDLE;
        }

        VkFormat formats[] = {
           VK_FORMAT_D32_SFLOAT,
           VK_FORMAT_D32_SFLOAT_S8_UINT,
           VK_FORMAT_D24_UNORM_S8_UINT
        };

        ctx.swap.depth_format = format_supported(formats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        VkAttachmentDescription da = { 0 };
        da.format = ctx.swap.depth_format;
        da.samples = VK_SAMPLE_COUNT_1_BIT;
        da.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        da.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        da.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        da.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        da.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        da.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference dar = { 0 };
        dar.attachment = 1;
        dar.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription ca = { 0 };
        ca.format = ctx.swap.image_format;
        ca.samples = VK_SAMPLE_COUNT_1_BIT;
        ca.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ca.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        ca.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ca.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ca.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ca.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference car = { 0 };
        car.attachment = 0;
        car.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = { 0 };
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &car;
        subpass.pDepthStencilAttachment = &dar;

        VkSubpassDependency dependency = { 0 };
        dependency.dstSubpass = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

        VkAttachmentDescription attachments[2] = { ca, da };
        VkRenderPassCreateInfo rpi = { 0 };
        rpi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpi.attachmentCount = 2;
        rpi.pAttachments = attachments;
        rpi.subpassCount = 1;
        rpi.pSubpasses = &subpass;
        rpi.dependencyCount = 1;
        rpi.pDependencies = &dependency;

        cassert(
            vkCreateRenderPass(
                ctx.dev.device,
                &rpi,
                NULL,
                &ctx.swap.renderpass
            ) == VK_SUCCESS,
            "swap_t failed to create renderpass!\n"
        );
    }

    // Clean up old resources
    for (u32 i = 0; i < old_image_count; i++) {
        if (ctx.swap.image_views[i] != VK_NULL_HANDLE) {
            vkDestroyImageView(ctx.dev.device, ctx.swap.image_views[i], NULL);
        }
        if (ctx.swap.depth_views[i] != VK_NULL_HANDLE) {
            vkDestroyImageView(ctx.dev.device, ctx.swap.depth_views[i], NULL);
        }
        if (ctx.swap.depth_images[i] != VK_NULL_HANDLE) {
            vkDestroyImage(ctx.dev.device, ctx.swap.depth_images[i], NULL);
        }
        if (ctx.swap.depth_memories[i] != VK_NULL_HANDLE) {
            vkFreeMemory(ctx.dev.device, ctx.swap.depth_memories[i], NULL);
        }
        if (ctx.swap.framebuffers[i] != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(ctx.dev.device, ctx.swap.framebuffers[i], NULL);
        }
    }

    // Allocate and initialize new resources
    VkImageView* new_image_views = realloc(ctx.swap.image_views, sizeof(VkImageView) * ctx.swap.image_count);
    cassert(new_image_views, "failed to reallocate memory for image views!");
    ctx.swap.image_views = new_image_views;
    memset(ctx.swap.image_views, 0, sizeof(VkImageView) * ctx.swap.image_count);

    VkFramebuffer* new_framebuffers = realloc(ctx.swap.framebuffers, sizeof(VkFramebuffer) * ctx.swap.image_count);
    cassert(new_framebuffers, "failed to reallocate memory for framebuffers!");
    ctx.swap.framebuffers = new_framebuffers;
    memset(ctx.swap.framebuffers, 0, sizeof(VkFramebuffer) * ctx.swap.image_count);

    VkImage* new_depth_images = realloc(ctx.swap.depth_images, sizeof(VkImage) * ctx.swap.image_count);
    cassert(new_depth_images, "failed to reallocate memory for depth images!");
    ctx.swap.depth_images = new_depth_images;
    memset(ctx.swap.depth_images, 0, sizeof(VkImage) * ctx.swap.image_count);

    VkImageView* new_depth_views = realloc(ctx.swap.depth_views, sizeof(VkImageView) * ctx.swap.image_count);
    cassert(new_depth_views, "failed to reallocate memory for depth views!");
    ctx.swap.depth_views = new_depth_views;
    memset(ctx.swap.depth_views, 0, sizeof(VkImageView) * ctx.swap.image_count);

    VkDeviceMemory* new_depth_memories = realloc(ctx.swap.depth_memories, sizeof(VkDeviceMemory) * ctx.swap.image_count);
    cassert(new_depth_memories, "failed to reallocate memory for depth memories!");
    ctx.swap.depth_memories = new_depth_memories;
    memset(ctx.swap.depth_memories, 0, sizeof(VkDeviceMemory) * ctx.swap.image_count);

    VkImageViewCreateInfo view_inf = { 0 };
    VkImageCreateInfo img_inf = { 0 };

    for (u32 i = 0; i < ctx.swap.image_count; i++) {
        // Create image views
        memset(&view_inf, 0, sizeof(VkImageViewCreateInfo));
        view_inf.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_inf.image = ctx.swap.images[i];
        view_inf.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_inf.format = ctx.swap.image_format;
        view_inf.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_inf.subresourceRange.baseMipLevel = 0;
        view_inf.subresourceRange.levelCount = 1;
        view_inf.subresourceRange.baseArrayLayer = 0;
        view_inf.subresourceRange.layerCount = 1;

        cassert(
            vkCreateImageView(
                ctx.dev.device,
                &view_inf,
                NULL,
                &ctx.swap.image_views[i]
            ) == VK_SUCCESS,
            "swap_t failed to create swap images!\n"
        );

        // Create depth images
        memset(&img_inf, 0, sizeof(VkImageCreateInfo));
        img_inf.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        img_inf.imageType = VK_IMAGE_TYPE_2D;
        img_inf.extent.width = ctx.swap.extent.width;
        img_inf.extent.height = ctx.swap.extent.height;
        img_inf.extent.depth = 1;
        img_inf.mipLevels = 1;
        img_inf.arrayLayers = 1;
        img_inf.format = ctx.swap.depth_format;
        img_inf.tiling = VK_IMAGE_TILING_OPTIMAL;
        img_inf.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        img_inf.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        img_inf.samples = VK_SAMPLE_COUNT_1_BIT;
        img_inf.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        img_inf.flags = 0;

        cassert(
            vkCreateImage(
                ctx.dev.device,
                &img_inf,
                NULL,
                &ctx.swap.depth_images[i]
            ) == VK_SUCCESS,
            "vkimage_t failed to create depth image!\n"
        );

        VkMemoryRequirements mem_req = { 0 };
        vkGetImageMemoryRequirements(
            ctx.dev.device,
            ctx.swap.depth_images[i],
            &mem_req
        );

        VkMemoryAllocateInfo alloc = { 0 };
        alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc.allocationSize = mem_req.size;
        alloc.memoryTypeIndex = get_mem_type(mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        cassert(
            vkAllocateMemory(
                ctx.dev.device,
                &alloc,
                NULL,
                &ctx.swap.depth_memories[i]
            ) == VK_SUCCESS,
            "vkimage_t failed to allocate memory\n"
        );

        cassert(
            vkBindImageMemory(
                ctx.dev.device,
                ctx.swap.depth_images[i],
                ctx.swap.depth_memories[i],
                0
            ) == VK_SUCCESS,
            "vkimage_t failed to bind image memory!\n"
        );

        memset(&view_inf, 0, sizeof(VkImageViewCreateInfo));
        view_inf.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_inf.image = ctx.swap.depth_images[i];
        view_inf.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_inf.format = ctx.swap.depth_format;
        view_inf.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view_inf.subresourceRange.baseMipLevel = 0;
        view_inf.subresourceRange.levelCount = 1;
        view_inf.subresourceRange.baseArrayLayer = 0;
        view_inf.subresourceRange.layerCount = 1;

        cassert(
            vkCreateImageView(
                ctx.dev.device,
                &view_inf,
                NULL,
                &ctx.swap.depth_views[i]
            ) == VK_SUCCESS,
            "swap_t failed to create depth images!\n"
        );

        VkImageView attachments[2] = {
            ctx.swap.image_views[i],
            ctx.swap.depth_views[i]
        };

        VkFramebufferCreateInfo framebuffer_info = { 0 };
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = ctx.swap.renderpass;
        framebuffer_info.attachmentCount = 2;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = ctx.swap.extent.width;
        framebuffer_info.height = ctx.swap.extent.height;
        framebuffer_info.layers = 1;

        cassert(
            vkCreateFramebuffer(
                ctx.dev.device,
                &framebuffer_info,
                NULL,
                &ctx.swap.framebuffers[i]
            ) == VK_SUCCESS,
            "swap_t failed to create framebuffer!\n"
        );
    }

    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (ctx.swap.finished_semaphores[i]) {
            vkDestroySemaphore(ctx.dev.device, ctx.swap.finished_semaphores[i], NULL);
            ctx.swap.finished_semaphores[i] = VK_NULL_HANDLE;
        }

        if (ctx.swap.available_semaphores[i]) {
            vkDestroySemaphore(ctx.dev.device, ctx.swap.available_semaphores[i], NULL);
            ctx.swap.available_semaphores[i] = VK_NULL_HANDLE;
        }

        if (ctx.swap.in_flight_fences[i]) {
            vkDestroyFence(ctx.dev.device, ctx.swap.in_flight_fences[i], NULL);
            ctx.swap.in_flight_fences[i] = VK_NULL_HANDLE;
        }
    }

    VkFence* new_images_in_flight = realloc(ctx.swap.images_in_flight, sizeof(VkFence) * ctx.swap.image_count);
    cassert(new_images_in_flight, "failed to reallocate memory for images in flight!");
    ctx.swap.images_in_flight = new_images_in_flight;
    memset(ctx.swap.images_in_flight, 0, sizeof(VkFence) * ctx.swap.image_count);

    VkSemaphoreCreateInfo semaphore_info = { 0 };
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info = { 0 };
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        cassert(
            vkCreateSemaphore(
                ctx.dev.device,
                &semaphore_info,
                NULL,
                &ctx.swap.available_semaphores[i]
            ) == VK_SUCCESS,
            "swap_t failed to create available semaphore\n"
        );

        cassert(
            vkCreateSemaphore(
                ctx.dev.device,
                &semaphore_info,
                NULL,
                &ctx.swap.finished_semaphores[i]
            ) == VK_SUCCESS,
            "swap_t failed to create finished semaphore\n"
        );

        cassert(
            vkCreateFence(
                ctx.dev.device,
                &fence_info,
                NULL,
                &ctx.swap.in_flight_fences[i]
            ) == VK_SUCCESS,
            "swap_t failed to create in flight fence\n"
        );
    }
}

VkResult vk_swap_submit() {
    // Wait for the fence if the image in flight is not null
    if (ctx.swap.images_in_flight[ctx.state.image_index] != VK_NULL_HANDLE) {
        VkFence fence = ctx.swap.images_in_flight[ctx.state.image_index];
        vkWaitForFences(
            ctx.dev.device,
            1,
            &fence,
            VK_TRUE,
            UINT64_MAX
        );
    }

    // Update the image in flight for the current image index
    ctx.swap.images_in_flight[ctx.state.image_index] =
        ctx.swap.in_flight_fences[ctx.swap.current_frame];

    // Set up the submit info for the queue submission
    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {
        ctx.swap.available_semaphores[ctx.swap.current_frame]
    };
    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
    };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = ctx.dev.cmd_buffers;

    VkSemaphore signal_semaphores[] = {
        ctx.swap.finished_semaphores[ctx.swap.current_frame]
    };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    // Reset the fence for the current frame
    vkResetFences(
        ctx.dev.device,
        1,
        &ctx.swap.in_flight_fences[ctx.swap.current_frame]
    );

    // Submit the command buffer to the queue
    cassert(
        vkQueueSubmit(
            ctx.dev.graphics_queue,
            1,
            &submit_info,
            ctx.swap.in_flight_fences[ctx.swap.current_frame]
        ) == VK_SUCCESS,
        "failed to submit command buffer!\n"
    );

    // Set up the present info for the swapchain presentation
    VkPresentInfoKHR present_info = { 0 };
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swapchains[] = { ctx.swap.new_swap };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &ctx.state.image_index;

    // Present the image to the swapchain
    VkResult result = vkQueuePresentKHR(
        ctx.dev.present_queue,
        &present_info
    );

    // Update the current frame
    ctx.swap.current_frame = (ctx.swap.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

VkResult vk_swap_acquire_next() {
    vkWaitForFences(
        ctx.dev.device,
        1,
        &ctx.swap.in_flight_fences[ctx.swap.current_frame],
        VK_TRUE,
        UINT64_MAX
    );

    VkResult result = vkAcquireNextImageKHR(
        ctx.dev.device,
        ctx.swap.new_swap,
        UINT64_MAX,
        ctx.swap.available_semaphores[ctx.swap.current_frame],
        VK_NULL_HANDLE,
        &ctx.state.image_index
    );

    return result;
}


/*--------------------------------- WINDOW ----------------------------------*/

#ifdef _WIN32

#include <Windows.h>
#include <vulkan/vulkan_win32.h>

static void vk_win_sync() {
    if (!ctx.win.handles[1]) return;
    HWND hwnd = (HWND)ctx.win.handles[1];

    RECT    rect;
    POINT   pt = { 0, 0 };

    if (GetClientRect(hwnd, &rect)) {
        ClientToScreen(hwnd, &pt);

        u32 px = (u32)pt.x;
        u32 py = (u32)pt.y;
        u32 w = (u32)(rect.right - rect.left);
        u32 h = (u32)(rect.bottom - rect.top);

        if (w == ctx.win.extent[0] && h == ctx.win.extent[1]) return;

        ctx.win.position[0] = px;
        ctx.win.position[1] = py;
        ctx.win.extent[0]   = w;
        ctx.win.extent[1]   = h;
        
        if (w != 0 || h != 0) {
            if (ctx.dev.device) {
                vkDeviceWaitIdle(ctx.dev.device);
                if (ctx.swap.new_swap) {
                    vk_swapchain();
                }
            }
        }
    }
}

static LRESULT CALLBACK wnd_proc(
    HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
    case WM_CLOSE:
        ctx.win_closing = true;
        return 0;
    case WM_DWMWINDOWMAXIMIZEDCHANGE:
        vk_win_sync();
        break;
    case WM_SIZE:
        vk_win_sync();
        break;
    case WM_EXITSIZEMOVE:
        vk_win_sync();
        break;
    case WM_DESTROY:
        ctx.win_closing = true;
        return 0;
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}


static void vk_win(const char* app_name) {
    wchar_t buff[20] = { 0 };
    size_t len = mbstowcs(NULL, app_name, 0);
    if (len == (size_t)(-1)) return;
    mbstowcs(buff, app_name, len + 1);

    ctx.win.handles[0] = GetModuleHandle(NULL);  // hInstance

    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wnd_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = ctx.win.handles[0];
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"trigon_win_t";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    cassert(RegisterClassEx(&wc), "failed to register window class for WIN32");

    ctx.win.handles[1] = CreateWindowEx(
        0,
        wc.lpszClassName,
        buff,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL, NULL,
        ctx.win.handles[0],
        NULL
    );

    cassert(ctx.win.handles[1], "failed to create window for WIN32");

    ctx.win.position[0] = CW_USEDEFAULT;
    ctx.win.position[1] = CW_USEDEFAULT;
    ctx.win.extent[0] = 800;
    ctx.win.extent[1] = 600;

    ShowWindow((HWND)ctx.win.handles[1], SW_SHOW);
   
    VkWin32SurfaceCreateInfoKHR surface_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .hinstance = ctx.win.handles[0],
        .hwnd = ctx.win.handles[1]
    };

    VkResult result = vkCreateWin32SurfaceKHR(
        ctx.dev.instance,
        &surface_info,
        NULL,
        &ctx.dev.surface
    );
    cassert(result == VK_SUCCESS, "failed to create Vulkan surface");
}
#endif


/*---------------------------------- VKCTX ----------------------------------*/

void vkctx_new(cstr_t app_name, version_t app_version) {
    vk_instance(app_name, app_version);
    vk_win(app_name);
    vk_device();
    vk_swapchain();

  
}

void vkctx_del() {
    vkDeviceWaitIdle(ctx.dev.device);

    vkFreeCommandBuffers(
        ctx.dev.device,
        ctx.dev.cmd_pool,
        MAX_FRAMES_IN_FLIGHT,
        ctx.dev.cmd_buffers
    );

    // Destroy framebuffers
    for (u32 i = 0; i < ctx.swap.image_count; i++) {
        if (ctx.swap.framebuffers[i] != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(ctx.dev.device, ctx.swap.framebuffers[i], NULL);
            ctx.swap.framebuffers[i] = VK_NULL_HANDLE;
        }
    }

    // Destroy image views
    for (u32 i = 0; i < ctx.swap.image_count; i++) {
        if (ctx.swap.image_views[i] != VK_NULL_HANDLE) {
            vkDestroyImageView(ctx.dev.device, ctx.swap.image_views[i], NULL);
            ctx.swap.image_views[i] = VK_NULL_HANDLE;
        }
    }

    // Destroy depth views and images
    for (u32 i = 0; i < ctx.swap.image_count; i++) {
        if (ctx.swap.depth_views[i] != VK_NULL_HANDLE) {
            vkDestroyImageView(ctx.dev.device, ctx.swap.depth_views[i], NULL);
            ctx.swap.depth_views[i] = VK_NULL_HANDLE;
        }

        if (ctx.swap.depth_images[i] != VK_NULL_HANDLE) {
            vkDestroyImage(ctx.dev.device, ctx.swap.depth_images[i], NULL);
            ctx.swap.depth_images[i] = VK_NULL_HANDLE;
        }

        if (ctx.swap.depth_memories[i] != VK_NULL_HANDLE) {
            vkFreeMemory(ctx.dev.device, ctx.swap.depth_memories[i], NULL);
            ctx.swap.depth_memories[i] = VK_NULL_HANDLE;
        }
    }

    // Destroy semaphores
    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (ctx.swap.finished_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(ctx.dev.device, ctx.swap.finished_semaphores[i], NULL);
            ctx.swap.finished_semaphores[i] = VK_NULL_HANDLE;
        }

        if (ctx.swap.available_semaphores[i] != VK_NULL_HANDLE) {
            vkDestroySemaphore(ctx.dev.device, ctx.swap.available_semaphores[i], NULL);
            ctx.swap.available_semaphores[i] = VK_NULL_HANDLE;
        }
    }

    // Destroy fences
    for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (ctx.swap.in_flight_fences[i] != VK_NULL_HANDLE) {
            vkDestroyFence(ctx.dev.device, ctx.swap.in_flight_fences[i], NULL);
            ctx.swap.in_flight_fences[i] = VK_NULL_HANDLE;
        }
    }

    // Free images in flight array
    if (ctx.swap.images_in_flight != NULL) {
        free(ctx.swap.images_in_flight);
        ctx.swap.images_in_flight = NULL;
    }

    // Destroy render pass
    if (ctx.swap.renderpass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(ctx.dev.device, ctx.swap.renderpass, NULL);
        ctx.swap.renderpass = VK_NULL_HANDLE;
    }

    // Destroy swapchain
    if (ctx.swap.new_swap != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(ctx.dev.device, ctx.swap.new_swap, NULL);
        ctx.swap.new_swap = VK_NULL_HANDLE;
    }

    // Clean up arrays
    if (ctx.swap.image_views != NULL) {
        free(ctx.swap.image_views);
        ctx.swap.image_views = NULL;
    }

    if (ctx.swap.framebuffers != NULL) {
        free(ctx.swap.framebuffers);
        ctx.swap.framebuffers = NULL;
    }

    if (ctx.swap.depth_images != NULL) {
        free(ctx.swap.depth_images);
        ctx.swap.depth_images = NULL;
    }

    if (ctx.swap.depth_views != NULL) {
        free(ctx.swap.depth_views);
        ctx.swap.depth_views = NULL;
    }

    if (ctx.swap.depth_memories != NULL) {
        free(ctx.swap.depth_memories);
        ctx.swap.depth_memories = NULL;
    }

    vkDestroyCommandPool(ctx.dev.device, ctx.dev.cmd_pool, NULL);

    vkDestroyDevice(ctx.dev.device, NULL);

    vkDestroySurfaceKHR(ctx.dev.instance, ctx.dev.surface, NULL);

    HWND hwnd           = (HWND)ctx.win.handles[1];
    HINSTANCE hinstance = (HINSTANCE)ctx.win.handles[0];
    DestroyWindow(hwnd);
    UnregisterClass(L"trigon_win_t", hinstance);

#ifdef _DEBUG
    destroy_debug_util(ctx.dev.instance, ctx.dev.debugger, NULL);
#endif

    vkDestroyInstance(ctx.dev.instance, NULL);

    memset(&ctx, 0, sizeof(vkctx_t));
}

VkCommandBuffer vkctx_single_cmd_begin() {
    VkCommandBufferAllocateInfo info = {0};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = ctx.dev.cmd_pool;
    info.commandBufferCount = 1;

    VkCommandBuffer cmd;
    VkResult result = vkAllocateCommandBuffers(ctx.dev.device, &info, &cmd);
    cassert(result == VK_SUCCESS, "failed to allocate command buffer!");

    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    result = vkBeginCommandBuffer(cmd, &begin_info);
    cassert(result == VK_SUCCESS, "failed to begin command buffer!");

    return cmd;
}

void vkctx_single_cmd_end(VkCommandBuffer cmd) {
    VkResult result = vkEndCommandBuffer(cmd);
    cassert(result == VK_SUCCESS, "failed to end command buffer!");

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd;

    result = vkQueueSubmit(ctx.dev.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    cassert(result == VK_SUCCESS, "failed to submit command buffer!");

    result = vkQueueWaitIdle(ctx.dev.graphics_queue);
    cassert(result == VK_SUCCESS, "failed to wait for queue to idle!");

    vkFreeCommandBuffers(ctx.dev.device, ctx.dev.cmd_pool, 1, &cmd);
}

VkCommandBuffer vkctx_current_cmd() {
    return ctx.dev.cmd_buffers[ctx.swap.current_frame];
}

VkResult vkctx_frame_begin() {
    cassert(!ctx.state.frame_in_progress, "frame already started!\n");

    VkResult result = vk_swap_acquire_next();
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vk_swapchain();
        return result;
    }

    cassert(
        result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
        "failed to acquire swapchain image index!"
    );
    ctx.state.frame_in_progress = true;

    VkCommandBuffer buff = ctx.dev.cmd_buffers[ctx.swap.current_frame];
    VkCommandBufferBeginInfo begin_info = {0};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    cassert(
        vkBeginCommandBuffer(buff, &begin_info) == VK_SUCCESS,
        "failed to begin recording command buffer!"
    );

    VkRenderPassBeginInfo renderpass_info = { 0 };
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpass_info.renderPass = ctx.swap.renderpass;
    renderpass_info.framebuffer = ctx.swap.framebuffers[ctx.state.image_index];
    renderpass_info.renderArea.offset = (VkOffset2D){ 0, 0 };
    renderpass_info.renderArea.extent = ctx.swap.extent;

    VkClearValue clear_values[2] = { 0 };
    clear_values[0].color = (VkClearColorValue){ {0.01f, 0.01f, 0.01f, 1.0f} };
    clear_values[1].depthStencil = (VkClearDepthStencilValue){ 1.0f, 0 };

    renderpass_info.clearValueCount = 2;
    renderpass_info.pClearValues = clear_values;

    vkCmdBeginRenderPass(
        buff,
        &renderpass_info,
        VK_SUBPASS_CONTENTS_INLINE
    );

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)ctx.swap.extent.width;
    viewport.height = (float)ctx.swap.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor = { {0, 0}, ctx.swap.extent };
    vkCmdSetViewport(buff, 0, 1, &viewport);
    vkCmdSetScissor(buff, 0, 1, &scissor);

    return VK_SUCCESS;
}

VkResult vkctx_frame_end() {
    cassert(ctx.state.frame_in_progress, "frame was not started\n");

    VkCommandBuffer buff = ctx.dev.cmd_buffers[ctx.swap.current_frame];
    vkCmdEndRenderPass(buff);

    cassert(
        vkEndCommandBuffer(buff) == VK_SUCCESS,
        "failed to record command buffer!\n"
    );

    VkCommandBuffer buffer = buff;
    VkResult result = vk_swap_submit(&buffer);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        vk_swapchain();
    }

    ctx.state.frame_in_progress = false;
    ctx.swap.current_frame = (ctx.swap.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

vkctx_t* vkctx_get() {return &ctx;}

void vkctx_poll() {
#ifdef _WIN32
    MSG msg = { 0 };

    if (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif
}