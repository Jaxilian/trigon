#include "vkdef.h"

void vkimage_new(vkimage_info_t* info, vkimage_t* out) {
	VkImageCreateInfo image_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.extent.width = info->width,
		.extent.height = info->height,
		.extent.depth = 1,
		.mipLevels = 1,
		.arrayLayers = 1,
		.format = info->format,
		.tiling = info->tiling,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.usage = info->usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.flags = 0
	};

	if (vkCreateImage(
		info->device,
		&image_info,
		NULL,
		&out->image) != VK_SUCCESS) {
		debug_err("failed to vkcreate image!\n");
	}

	VkMemoryRequirements memreq = { 0 };
	vkGetImageMemoryRequirements(
		info->device,
		out->image,
		&memreq
	);

	VkMemoryAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memreq.size,
		.memoryTypeIndex = vk_memory_type(
			info->physical,
			memreq.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		)
	};

	if (vkAllocateMemory(
		info->device,
		&alloc_info,
		NULL,
		&out->memory
	) != VK_SUCCESS) {
		debug_err("failed to allocate image memory!");
	}

	vkBindImageMemory(
		info->device,
		out->image,
		out->memory,
		0
	);
}

void vkimage_del(VkDevice device, vkimage_t* img) {
	vkUnmapMemory(device, img->memory);
	vkDestroyImage(device, img, NULL);
}