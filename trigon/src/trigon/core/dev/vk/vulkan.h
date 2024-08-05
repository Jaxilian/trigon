#ifndef TGVULKAN_H
#define TGVULKAN_H
#include "trigon/core/std/std.h"

/* ------------------------------- VULKAN -----------------------------------*/


#ifdef _USING_VULKAN_SDK
#include <vulkan/vulkan.h>
#else
typedef vptr_t VkSurfaceKHR;
typedef vptr_t VkSurfaceKHR;
typedef vptr_t VkInstance;
typedef vptr_t VkDebugUtilsMessengerEXT;
typedef vptr_t VkPhysicalDevice;
typedef vptr_t VkCommandPool;
typedef vptr_t VkDevice;
typedef vptr_t VkQueue;
typedef vptr_t VkCommandBuffer;
typedef vptr_t VkSwapchainKHR;
typedef vptr_t VkImage;
typedef vptr_t VkImageView;
typedef vptr_t VkFormat;
typedef vptr_t VkExtent2D;
typedef vptr_t VkRenderPass;
typedef vptr_t VkDeviceMemory;
typedef vptr_t VkFramebuffer;
typedef vptr_t VkSemaphore;
typedef vptr_t VkFence;
typedef vptr_t VkPresentModeKHR;
typedef vptr_t VkShaderModule;
typedef vptr_t VkPipelineLayout;
typedef vptr_t VkPipeline;
typedef vptr_t VkSurfaceFormatKHR;
typedef vptr_t VkBuffer;
typedef uint32_t VkResult;
typedef uint32_t VkMemoryPropertyFlags;
typedef uint32_t VkDeviceSize;
typedef uint32_t VkBufferUsageFlags;
typedef uint32_t VkImageTiling;
typedef uint32_t VkImageUsageFlags;
typedef uint32_t VkImageLayout;
#endif




/* ------------------------------ DEFINES -----------------------------------*/


#define MAX_FRAMES_IN_FLIGHT 2


/* --------------------------------- MISC -----------------------------------*/


class swapsupp_t {
public:
    VkSurfaceCapabilitiesKHR capabilities = {};
    VkSurfaceFormatKHR* formats = VK_NULL_HANDLE;
    VkPresentModeKHR* presents = VK_NULL_HANDLE;
    u32 format_count = 0;
    u32 present_count = 0;

    swapsupp_t(VkPhysicalDevice& gpu);
    ~swapsupp_t();
};

class qfi_t {
public:
    u32 gfam = 0;
    u32 pfam = 0;
    b8  gfam_valued = false;
    b8  pfam_valued = false;

    qfi_t(VkPhysicalDevice& gpu);
    ~qfi_t();
};


/* --------------------------- VULKAN DEVICE --------------------------------*/


class vkdev_t {
public:
	vkdev_t(cstr_t app_name, version_t app_ver);
	~vkdev_t();

    static const cstr_t     ENGINE_NAME;
    static const version_t  ENGINE_VERSION;
    static cstr_t           APP_NAME;
    static version_t        APP_VERSION;

    VkInstance       instance       = VK_NULL_HANDLE;
    VkSurfaceKHR     surface        = VK_NULL_HANDLE;
    VkPhysicalDevice physical       = VK_NULL_HANDLE;
    VkDevice         device         = VK_NULL_HANDLE;
    VkQueue          graphics_queue = VK_NULL_HANDLE;
    VkQueue          present_queue  = VK_NULL_HANDLE;
    VkCommandPool    cmdpool        = VK_NULL_HANDLE;
   

    u32 gfam = 0;
    u32 pfam = 0;

    static vkdev_t& ref() {
        cassert(this_ptr, "vkdev_t.ptr was nullptr!\n");
        return *this_ptr;
    }

    b8  extension_supported(VkPhysicalDevice pd, cstr_t name);
    u32 get_mem_type(u32 type, VkMemoryPropertyFlags properties);

    VkFormat format_supported(
        const VkFormat* candidates,
        u32 candidate_count,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT vkdbg = VK_NULL_HANDLE;
#endif

    void create_instance();
    void create_physical();
    void create_logical();

private:
    static vkdev_t* this_ptr;

    void destroy_instance();
    void destroy_physical();
    void destroy_logical();
};


/* -------------------------------- IMAGE -----------------------------------*/


class vkimage_t {
public:
    VkFormat        format = (VkFormat)0;
    VkImage         img = VK_NULL_HANDLE;
    VkDeviceMemory  mem = VK_NULL_HANDLE;
    VkImageView     view = VK_NULL_HANDLE;
    bool            swapimg = false;

    void create_from_info(VkImageCreateInfo inf, VkMemoryPropertyFlags prop);
    void create_view(VkImageViewCreateInfo& inf);
    void destroy();

    vkimage_t() {};
    vkimage_t(VkFormat frm, VkImageCreateInfo inf, VkMemoryPropertyFlags prop);
    ~vkimage_t();

    static VkFormat             depth_format();
    static VkSurfaceFormatKHR   swap_format();
};


/* --------------------------------- SWAP -----------------------------------*/


class swap_t {
public:
    void create();
    void destroy();

    VkSwapchainKHR	old_swap = VK_NULL_HANDLE;
    VkSwapchainKHR	new_swap = VK_NULL_HANDLE;
    VkExtent2D		extent = {};
    VkRenderPass	renderpass = VK_NULL_HANDLE;
    u32		        images_count = 0;
    vkimage_t*      images = {};

    u32             depth_images_count = 0;
    vkimage_t*      depth_images = {};
    u32             framebuffer_count = 0;
    VkFramebuffer*  framebuffers = nullptr;
    VkSemaphore     available_semaphores[MAX_FRAMES_IN_FLIGHT] = {};
    VkSemaphore     finished_semaphores[MAX_FRAMES_IN_FLIGHT] = {};
    VkFence         in_flight_fences[MAX_FRAMES_IN_FLIGHT] = {};
    VkFence*        images_in_flight = VK_NULL_HANDLE;
    u32		        current_frame = 0;

    VkResult submit(const VkCommandBuffer* buffers, u32* img_idx);
    VkResult acquire_next(u32* img_idx);


     swap_t(vkdev_t& dev);
    ~swap_t();

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

    vkdev_t&        device;
};


/* --------------------------------- CMDR -----------------------------------*/


class vkcmdr_t {
public:
     vkcmdr_t(vkdev_t& dev, swap_t& swp);
    ~vkcmdr_t();

    VkCommandBuffer cmd_begin();
    void cmd_end(VkCommandBuffer buff);

    VkResult frame_begin();
    VkResult frame_end();

    inline VkCommandBuffer cmdbuff() const {
        return cmdbuffers[frame_index];
    }

private:
    vkdev_t&        device;
    swap_t&         swap;

    b8  frame_started = false;
    u32	frame_index = 0;
    u32	image_index = 0;
    u32 cmdbuff_count = 0;

    VkCommandBuffer* cmdbuffers = VK_NULL_HANDLE;
};

#endif // !TGVULKAN_H
