#include "texture.h"
#include "trigon/core/utils/debug.h"

#include "trigon/system/rendr.h"

#define STB_IMAGE_IMPLEMENTATION
#include <trigon/core/ven/stb_image.h>

typedef struct {
	VkImageView view;
	vkimage_t	image;
	VkSampler	sampler;
} texture_t;

static texture_t tex = { 0 };

texture_info_t	texture_read(vpath_t path) {
	texture_info_t info = { 0 };
	stbi_uc* pixels = stbi_load(
		path,
		&info.width,
		&info.height,
		&info.channels,
		STBI_rgb_alpha
	);
	info.pixel_count = info.width * info.height * STBI_rgb_alpha;
	
	if (!pixels) {
		debug_err("failed to load texture data from file: %s\n", path);
	}
	else {
		vpath_t name = { 0 };
		vfs_filename(path, name, false);
		debug_log("loaded texture data: %s\n", name);
	}

	info.pixels = malloc(sizeof(uc_t) * info.pixel_count);
	memcpy_s(info.pixels, sizeof(uc_t) * info.pixel_count, pixels, sizeof(stbi_uc) * info.pixel_count);
	stbi_image_free(pixels);
	return info;
}

static void bind_img(texture_info_t* info, texture_t* tex) {
	VkBuffer		staging_buffer = { 0 };
	VkDeviceMemory	staging_memory = { 0 };
	rendr_t* renderer = rendr_get();

	vkbuffer_new(
		&renderer->device,
		info->pixel_count,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&staging_buffer,
		&staging_memory
	);

	void* data = NULL;

	vkMapMemory(
		renderer->device.device,
		staging_memory,
		0,
		info->pixel_count,
		0,
		&data
	);
	memcpy_s(data, info->pixel_count, info->pixels, info->pixel_count);
	vkUnmapMemory(renderer->device.device, staging_memory);

	free(info->pixels);
	info->pixels = NULL;


	vkimage_info_t	image_info = {
		.device = renderer->device.device,
		.physical = renderer->device.physical,
		.width = info->width,
		.height = info->height,
		.format = VK_FORMAT_R8G8B8A8_SRGB,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	};

	vkimage_new(&image_info, &tex->image);

	vkimage_transition_layout(
		&renderer->state,
		tex->image.image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	);

	vkimage_copy_buffer(
		&renderer->state,
		&tex->image.image,
		staging_buffer,
		info->width,
		info->height
	);

	vkimage_transition_layout(
		&renderer->state,
		tex->image.image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);

	vkDestroyBuffer(renderer->device.device, staging_buffer, NULL);
	vkFreeMemory(renderer->device.device, staging_memory, NULL);

}

static void bind_view(texture_info_t* info, texture_t* tex) {
	rendr_t* renderer = rendr_get();
	VkImageViewCreateInfo view_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = tex->image.image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_R8G8B8A8_SRGB,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1
	};
	if (vkCreateImageView(
		renderer->device.device,
		&view_info,
		NULL,
		&tex->view) != VK_SUCCESS) {
		debug_err("failed to create texture image view!\n");
	}
}

static void bind_sampler(texture_info_t* info, texture_t* tex) {
	rendr_t* renderer = rendr_get();
	VkPhysicalDeviceProperties properties = {0};
	vkGetPhysicalDeviceProperties(renderer->device.physical, &properties);


	VkSamplerCreateInfo sampinfo = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.anisotropyEnable = VK_TRUE,
		.maxAnisotropy = properties.limits.maxSamplerAnisotropy,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.minLod = 0.0f,
		.maxLod = 0.0f
	};

	if (vkCreateSampler(
		renderer->device.device,
		&sampinfo,
		NULL, 
		&tex->sampler) != VK_SUCCESS) {
		debug_err("failed to create texture sampler!");
	}
}

uint32_t texture_new(texture_info_t* info) {
	bind_img(info, &tex);
	bind_view(info, &tex);
	bind_sampler(info, &tex);
	return 0;
}

void texture_del() {
	rendr_t* renderer = rendr_get();
	vkDestroySampler(renderer->device.device, tex.sampler, NULL);
	vkDestroyImageView(renderer->device.device, tex.view, NULL);
	vkimage_del(renderer->device.device, &tex.image);
}

