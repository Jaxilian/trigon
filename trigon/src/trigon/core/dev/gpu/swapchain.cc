#include "gpu.h"
#
#include <stdlib.h>
#include <memory.h>
#include "trigon/core/dev/win/window.h"

void swap_t::destroy_old_swap() {
	if (old_swap) {
		vkDestroySwapchainKHR(
			vgpu_t::ref().handle,
			old_swap,
			NULL
		);
		old_swap = VK_NULL_HANDLE;
	}
}

void swap_t::destroy_swap() {
	if (new_swap) {
		vkDestroySwapchainKHR(
			vgpu_t::ref().handle,
			new_swap,
			NULL
		);
		new_swap = VK_NULL_HANDLE;
	}
}

void swap_t::destroy_imgs() {
	if (images) {
		for (u32 i = 0; i < images_count; i++) {
			images[i].destroy();
		}

		delete[] images;
		images = nullptr;
	}
}

void swap_t::destroy_render() {
	if (renderpass) {
		vkDestroyRenderPass(
			vgpu_t::ref().handle,
			renderpass,
			nullptr
		);

		renderpass = nullptr;
	}
}

void swap_t::destroy_depth() {
	if (depth_images) {
		delete[] depth_images;
		depth_images = nullptr;
	}
}

void  swap_t::destroy_framebuffers() {
	if (framebuffers) {
		for (u32 i = 0; i < framebuffer_count; i++) {
			vkDestroyFramebuffer(
				vgpu_t::ref().handle,
				framebuffers[i], 
				NULL
			);
		}

		delete[] framebuffers;
		framebuffers = nullptr;
	}
}

void  swap_t::destroy_sync_objects() {
	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDevice& gpu = vgpu_t::ref().handle;

		if (finished_semaphores[i]) {
			vkDestroySemaphore(gpu, finished_semaphores[i], nullptr);
			finished_semaphores[i] = VK_NULL_HANDLE;
		}

		if (available_semaphores[i]) {
			vkDestroySemaphore(gpu, available_semaphores[i], nullptr);
			available_semaphores[i] = VK_NULL_HANDLE;
		}
		if (in_flight_fences[i]) {
			vkDestroyFence(gpu, in_flight_fences[i], nullptr);
			in_flight_fences[i] = VK_NULL_HANDLE;
		}
	}
}

VkPresentModeKHR swap_t::get_present_mode(
	VkPresentModeKHR* available, u32 count) {

	for (u32 i = 0; i < count; i++) {
		if (available[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			debug_log("Use: Mailbox\n");
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

	debug_log("Use: V-Sync\n");
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swap_t::get_extent() {
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		gpu_t::ref().handle,
		window.surface,
		&capabilities
	);
	
	VkExtent2D extent = {};
	extent.width	= window.width();
	extent.height	= window.height();

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

void swap_t::create_swap() {
	old_swap = new_swap;

	swapsupp_t swapsupp(gpu_t::ref().handle);

	VkSurfaceFormatKHR	surface_format = vkimage_t::swap_format();
	VkPresentModeKHR	present_mode = get_present_mode(
		swapsupp.presents,
		swapsupp.present_count
	);

	extent = get_extent();

	images_count = swapsupp.capabilities.minImageCount + 1;
	if (swapsupp.capabilities.maxImageCount > 0 &&
		images_count > swapsupp.capabilities.maxImageCount) {
		images_count = swapsupp.capabilities.maxImageCount;
	}


	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.surface = window.surface;

	info.minImageCount		= images_count;
	info.imageFormat		= surface_format.format;
	info.imageColorSpace	= surface_format.colorSpace;
	info.imageExtent		= extent;
	info.imageArrayLayers	= 1;
	info.imageUsage			= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	qfi_t qfi(gpu_t::ref().handle);
	u32 qfis[] = { qfi.gfam, qfi.pfam };

	if (qfi.gfam != qfi.pfam) {
		info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		info.queueFamilyIndexCount = 2;
		info.pQueueFamilyIndices = qfis;
	}
	else {
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount = 0;      // Optional
		info.pQueueFamilyIndices = nullptr;  // Optional
	}

	info.preTransform = swapsupp.capabilities.currentTransform;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	info.presentMode = present_mode;
	info.clipped = VK_TRUE;

	info.oldSwapchain = old_swap == nullptr ? VK_NULL_HANDLE : new_swap;

	cassert(
		vkCreateSwapchainKHR(
			vgpu_t::ref().handle,
			&info,
			nullptr,
			&new_swap
		) == VK_SUCCESS, 
		"swap_t failed to create swapchain!\n"
	);

	destroy_old_swap();
}

void swap_t::create_imgs() {
	destroy_imgs();

	vkGetSwapchainImagesKHR(
		vgpu_t::ref().handle,
		new_swap,
		&images_count,
		nullptr
	);

	VkImage* imgs = new VkImage[images_count];

	vkGetSwapchainImagesKHR(
		vgpu_t::ref().handle,
		new_swap,
		&images_count,
		imgs
	);

	VkFormat format = vkimage_t::swap_format().format;
	images = new vkimage_t[images_count];

	for (u32 i = 0; i < images_count; i++) {
		images[i]			= {};
		images[i].img		= imgs[i];
		images[i].format	= format;
		images[i].swapimg	= true;
		VkImageViewCreateInfo view_inf = {};
		view_inf.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_inf.image = images[i].img;
		view_inf.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_inf.format = images[i].format;
		view_inf.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		view_inf.subresourceRange.baseMipLevel = 0;
		view_inf.subresourceRange.levelCount = 1;
		view_inf.subresourceRange.baseArrayLayer = 0;
		view_inf.subresourceRange.layerCount = 1;

		cassert(
			vkCreateImageView(
				vgpu_t::ref().handle,
				&view_inf,
				NULL,
				&images[i].view
			) == VK_SUCCESS,
			"swap_t failed to create swap images!\n"
		);

	}

	delete[] imgs;
}

void swap_t::create_render() {
	destroy_render();


}

void swap_t::create_depth() {

}

void swap_t::create_framebuffers() {

}

void swap_t::create_sync_objects() {

}

void swap_t::create() {
	create_swap();
	create_imgs();
	create_render();
	create_depth();
	create_framebuffers();
	create_sync_objects();
}

void swap_t::destroy() {
	destroy_old_swap();
	destroy_swap();
	destroy_imgs();
	destroy_render();
	destroy_depth();
	destroy_framebuffers();
	destroy_sync_objects();
}

swap_t::swap_t(window_t& _win) :window(_win){
	create();
}

swap_t::~swap_t() {
	destroy();
}