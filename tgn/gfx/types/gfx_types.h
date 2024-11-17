#ifndef GFX_TYPES_H
#define GFX_TYPES_H
#include "gfx/core/gfxctx.h"

typedef struct {
    VkDeviceSize size;
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags properties;
} gfx_buffer_info_t;

typedef struct {
    VkDeviceMemory  memory;
    VkBuffer        buffer;
} gfx_buffer_t;

VkResult gfx_buffer_new(gfx_buffer_info_t* info, gfx_buffer_t* out);
void gfx_buffer_del(gfx_buffer_t* buffer);

typedef struct {
    VkImage         image;
    VkDeviceMemory  memory;
    VkImageView     view;
    VkDescriptorSet desc_set; 
    uint32_t        width;
    uint32_t        height;
    uint32_t        mipmap_count;
    uint32_t        channels;
    VkFormat        format;
} gfx_image_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
    uint32_t mipmap_count;
}gfx_image_info_t;

VkResult    gfx_image_new(gfx_image_info_t* info, gfx_image_t* out);
VkResult    gfx_image_view_create(gfx_image_t* image, VkImageAspectFlags aspect);
void        gfx_image_transition(VkImageLayout new_layout, VkImageLayout old_layout, gfx_image_t* image);
void        gfx_image_insert_data(gfx_image_t* image, VkBuffer data);

void        gfx_image_texture(VkFormat format, uint32_t width, uint32_t height, uint32_t channels, uint32_t mipmap, const void* pixelData, gfx_image_t* out);

void        gfx_image_del(gfx_image_t* image);
#endif // !GFX_TYPES_H
