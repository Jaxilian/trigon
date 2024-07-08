#include "vkdef.h"
#include "trigon/core/utils/debug.h"

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

void vkimage_transition_layout(
	vkstate_t*		state,
	VkImage			image,
	VkFormat		format,
	VkImageLayout	oldlayout,
	VkImageLayout	newlayout
) {
	VkCommandBuffer cmdbuff = vkstate_single_cmd(state);

	VkImageMemoryBarrier barrier ={
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = oldlayout,
		.newLayout = newlayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.srcAccessMask = 0,
		.dstAccessMask = 0
	};

	VkPipelineStageFlags source			= { 0 };
	VkPipelineStageFlags destination	= { 0 };
	if (oldlayout == VK_IMAGE_LAYOUT_UNDEFINED && newlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		source = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldlayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newlayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		source = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destination = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		debug_err("unsupported layout transition!\n");
	}

	vkCmdPipelineBarrier(
		cmdbuff,
		source,
		destination,
		0,
		0, 
		NULL,
		0, NULL,
		1, 
		&barrier
	);

	vkstate_single_cmd_submit(state, cmdbuff);
}

void vkimage_copy_buffer(vkstate_t* state, vkimage_t* to, VkBuffer src, uint32_t width, uint32_t height) {
	VkCommandBuffer cmdbuff = vkstate_single_cmd(state);

	VkBufferImageCopy region = {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.imageSubresource.mipLevel = 0,
		.imageSubresource.baseArrayLayer = 0,
		.imageSubresource.layerCount = 1,
		.imageOffset = { 0, 0, 0 },
		.imageExtent = {
			width,
			height,
			1
		}
	};

	vkCmdCopyBufferToImage(
		cmdbuff,
		src,
		to->image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	vkstate_single_cmd_submit(state, cmdbuff);
}



void vkimage_del(VkDevice device, vkimage_t* img) {
	vkDestroyImage(device, img->image, NULL);
	vkFreeMemory(device, img->memory, NULL);
}