#include "vulkan.h"

void vkimage_t::create_from_info(VkImageCreateInfo inf, VkMemoryPropertyFlags prop) {
    cassert(
        vkCreateImage(
            vkdev_t::ref().device,
            &inf,
            NULL,
            &img
        ) == VK_SUCCESS,
        "vkimage_t failed to create image!\n"
    );


    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(
        vkdev_t::ref().device,
        img,
        &mem_req
    );

    VkMemoryAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc.allocationSize = mem_req.size;
    alloc.memoryTypeIndex = vkdev_t::ref().get_mem_type(
        mem_req.memoryTypeBits,
        prop
    );

    cassert(vkAllocateMemory(
        vkdev_t::ref().device,
        &alloc,
        nullptr,
        &mem
    ) == VK_SUCCESS,
        "vkimage_t failed to allocate memory\n"
    );

    cassert(vkBindImageMemory(
        vkdev_t::ref().device,
        img,
        mem,
        0) == VK_SUCCESS,
        "vkimage_t failed to bind image memory!\n"
    );
}
vkimage_t::vkimage_t(VkFormat frm, VkImageCreateInfo inf, VkMemoryPropertyFlags prop)
    : format(frm) {

    create_from_info(inf, prop);
}

void vkimage_t::destroy() {
    if (!swapimg && img) {
        vkDestroyImage(vkdev_t::ref().device, img, NULL);
        img = VK_NULL_HANDLE;
    }

    if (mem) {
        vkFreeMemory(vkdev_t::ref().device, mem, NULL);
        mem = VK_NULL_HANDLE;
    }

    if (view) {
        vkDestroyImageView(vkdev_t::ref().device, view, NULL);
        view = VK_NULL_HANDLE;
    }
}

vkimage_t::~vkimage_t() {
    destroy();
}


void vkimage_t::create_view(VkImageViewCreateInfo& inf) {
    cassert(
        vkCreateImageView(
            vkdev_t::ref().device,
            &inf,
            NULL,
            &view
        ) == VK_SUCCESS,
        "vkimage_t failed to create image view!\n"
    );
}


VkFormat vkimage_t::depth_format() {
    VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT, 
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    return vkdev_t::ref().format_supported(
        formats,
        3,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

VkSurfaceFormatKHR vkimage_t::swap_format() {

    swapsupp_t supp(vkdev_t::ref().physical);
  
    for (u32 i = 0; i < supp.format_count; i++) {
        if (supp.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            supp.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return supp.formats[i];
        }
    }

    debug_wrn("didn't find swap format, returns index [0]\n");
    return supp.formats[0];
}