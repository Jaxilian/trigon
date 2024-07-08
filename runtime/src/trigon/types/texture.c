#include "texture.h"
#include "trigon/core/utils/debug.h"
#include "trigon/core/graphics/vkdef.h"
#include "trigon/system/rendr.h"

#define STB_IMAGE_IMPLEMENTATION
#include <trigon/core/ven/stb_image.h>

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

uint32_t texture_new(texture_info_t* info) {
	VkBuffer		staging_buffer = { 0 };
	VkDeviceMemory	staging_memory = { 0 };
	rendr_t*		renderer = rendr_get();

	vkbuffer_new(
		&renderer->device,
		info->pixel_count,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer,
		staging_memory
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

	s
	free(info->pixels);
	info->pixels = NULL;
	return 0;
}

void texture_del() {

}

