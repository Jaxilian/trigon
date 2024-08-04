#pragma once
#include "trigon/core/std/std.h"
#include "trigon/core/dev/gpu/vk.h"

class window_t;

#define MAX_FRAMES_IN_FLIGHT 2

class vkinst_t {
public:

    void load(cstr_t app_name, version_t app_ver);

    VkInstance  vki = VK_NULL_HANDLE;


    static const cstr_t     ENGINE_NAME;
    static const version_t  ENGINE_VERSION;
    static cstr_t           APP_NAME;
    static version_t        APP_VERSION;

private:
#ifdef _DEBUG
    VkDebugUtilsMessengerEXT vkdbg = VK_NULL_HANDLE;
#endif

    vkinst_t();
    ~vkinst_t();

public:
    vkinst_t(const vkinst_t&) = delete;
    vkinst_t& operator=(const vkinst_t&) = delete;
    vkinst_t(vkinst_t&&) = delete;
    vkinst_t& operator=(vkinst_t&&) = delete;


    static vkinst_t& ref() {
        static vkinst_t instance;
        return instance;
    }
};

/* --------------------------------- GPU ------------------------------------*/


class gpu_t {
public:
    VkPhysicalDevice handle = VK_NULL_HANDLE;
    u32 gfam = 0;
    u32 pfam = 0;
    u32 get_mem_type(u32 type, VkMemoryPropertyFlags properties);

    VkFormat format_supported(
        const VkFormat* candidates,
        u32 candidate_count,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);

private:
     gpu_t();
    ~gpu_t();
public:
    gpu_t(const gpu_t&) = delete;
    gpu_t& operator=(const gpu_t&) = delete;
    gpu_t(gpu_t&&) = delete;
    gpu_t& operator=(gpu_t&&) = delete;

    static b8 extension_supported(VkPhysicalDevice pd, cstr_t name);

    static gpu_t& ref() {
        static gpu_t instance;
        return instance;
    }
};


/* --------------------------------- MISC -----------------------------------*/


class swapsupp_t {
public:
    VkSurfaceCapabilitiesKHR capabilities = {};
    VkSurfaceFormatKHR* formats = VK_NULL_HANDLE;
    VkPresentModeKHR* presents  = VK_NULL_HANDLE;
    u32 format_count    = 0;
    u32 present_count   = 0;

    swapsupp_t(VkPhysicalDevice& gpu);
    ~swapsupp_t();
};

class qfi_t {
public:
    u32 gfam    = 0;
    u32 pfam    = 0;
    b8  gfam_valued  = false;
    b8  pfam_valued   = false;

     qfi_t(VkPhysicalDevice& gpu);
    ~qfi_t();
};


/* --------------------------------- VGPU -----------------------------------*/


class vgpu_t {
public:
    VkDevice        handle          = VK_NULL_HANDLE;
    VkQueue         graphics_queue  = VK_NULL_HANDLE;
    VkQueue         present_queue   = VK_NULL_HANDLE;
    VkCommandPool   cmdpool         = VK_NULL_HANDLE;

private:

    vgpu_t();
    ~vgpu_t();

public:
    vgpu_t(const vgpu_t&) = delete;
    vgpu_t& operator=(const vgpu_t&) = delete;
    vgpu_t(vgpu_t&&) = delete;
    vgpu_t& operator=(vgpu_t&&) = delete;


    static vgpu_t& ref() {
        static vgpu_t instance;
        return instance;
    }
};


/* -------------------------------- IMAGE -----------------------------------*/

class vkimage_t {
public:
    VkFormat        format  = (VkFormat)0;
    VkImage         img     = VK_NULL_HANDLE;
    VkDeviceMemory  mem     = VK_NULL_HANDLE;
    VkImageView     view    = VK_NULL_HANDLE;
    bool            swapimg = false;

    void create_view(VkImageViewCreateInfo& inf);
    void destroy();

    vkimage_t() = default;
    vkimage_t(VkFormat frm, VkImageCreateInfo inf, VkMemoryPropertyFlags prop);
    ~vkimage_t();

    static VkFormat             depth_format();
    static VkSurfaceFormatKHR   swap_format();
};



/* --------------------------------- SWAP -----------------------------------*/



class swap_t {
public:
     swap_t(window_t& _win);
    ~swap_t();

    void create();
    void destroy();

    VkSwapchainKHR	old_swap        = VK_NULL_HANDLE;
    VkSwapchainKHR	new_swap        = VK_NULL_HANDLE;
    VkExtent2D		extent          = {};
    VkRenderPass	renderpass      = VK_NULL_HANDLE;
    u32		        images_count    = 0;
    vkimage_t*      images          = {};

    u32             deoth_images_count  = 0;
    vkimage_t*      depth_images        = {};
    u32             framebuffer_count   = 0;
    VkFramebuffer*  framebuffers        = nullptr;
    VkSemaphore     available_semaphores[MAX_FRAMES_IN_FLIGHT] = {};
    VkSemaphore     finished_semaphores[MAX_FRAMES_IN_FLIGHT] = {};
    VkFence         in_flight_fences[MAX_FRAMES_IN_FLIGHT] = {};
    VkFence*        images_in_flight = VK_NULL_HANDLE;
    u32		        current_frame = 0;
    window_t&       window;

private:
    void create_swap();
    void create_imgs();
    void create_render();
    void create_depth();
    void create_framebuffers();
    void create_sync_objects();

    void destroy_swap();
    void destroy_imgs();
    void destroy_render();
    void destroy_depth();
    void destroy_framebuffers();
    void destroy_sync_objects();
    void destroy_old_swap();

    static VkPresentModeKHR get_present_mode(
        VkPresentModeKHR* available, u32 count
    );

    VkExtent2D get_extent();

};