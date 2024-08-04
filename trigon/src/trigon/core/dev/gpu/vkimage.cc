#include "gpu.h"

vkimage_t::vkimage_t(
    VkFormat frm, VkImageCreateInfo inf, VkMemoryPropertyFlags prop)
    : format(frm) {

    cassert(
        vkCreateImage(
            vgpu_t::ref().handle,
            &inf,
            NULL,
            &img
            ) == VK_SUCCESS,
        "vkimage_t failed to create image!\n"
    );


    VkMemoryRequirements mem_req;
    vkGetImageMemoryRequirements(
        vgpu_t::ref().handle,
        img,
        &mem_req
    );

    VkMemoryAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc.allocationSize = mem_req.size;
    alloc.memoryTypeIndex = gpu_t::ref().get_mem_type(
        mem_req.memoryTypeBits,
        prop
    );

    cassert(vkAllocateMemory(
            vgpu_t::ref().handle,
            &alloc,
            nullptr,
            &mem
        ) == VK_SUCCESS,
        "vkimage_t failed to allocate memory\n"
    );

    cassert(vkBindImageMemory(
        vgpu_t::ref().handle,
        img,
        mem,
        0) == VK_SUCCESS,
        "vkimage_t failed to bind image memory!\n"
    );
}

void vkimage_t::destroy() {
    if (!swapimg && img) {
        vkDestroyImage(vgpu_t::ref().handle, img, NULL);
        img = VK_NULL_HANDLE;
    }

    if (mem) {
        vkFreeMemory(vgpu_t::ref().handle, mem, NULL);
        mem = VK_NULL_HANDLE;
    }

    if (view) {
        vkDestroyImageView(vgpu_t::ref().handle, view, NULL);
        view = VK_NULL_HANDLE;
    }
}

vkimage_t::~vkimage_t() {
    destroy();
}


void vkimage_t::create_view(VkImageViewCreateInfo& inf) {
    cassert(
        vkCreateImageView(
            vgpu_t::ref().handle,
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

    return gpu_t::ref().format_supported(
        formats,
        3,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

VkSurfaceFormatKHR vkimage_t::swap_format() {

    swapsupp_t supp(gpu_t::ref().handle);
  
    for (u32 i = 0; i < supp.format_count; i++) {
        if (supp.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            supp.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return supp.formats[i];
        }
    }

    debug_wrn("didn't find swap format, returns index [0]\n");
    return supp.formats[0];
}