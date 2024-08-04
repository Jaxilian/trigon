#include "gpu.h"
#
#include <stdlib.h>
#include <memory.h>
#include "trigon/core/dev/win/window.h"

VkResult swap_t::submit(const VkCommandBuffer* buffers, u32* img_idx) {

	VkDevice& dev = vgpu_t::ref().handle;
	if (images_in_flight[*img_idx] != VK_NULL_HANDLE) {
		VkFence fence = images_in_flight[*img_idx];
		vkWaitForFences(
			dev,
			1,
			&fence,
			VK_TRUE,
			UINT64_MAX
		);
	}

	images_in_flight[*img_idx] = in_flight_fences[current_frame];

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore wait_sema[] = { available_semaphores[current_frame] };
	VkPipelineStageFlags wait_stages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT 
	};

	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = wait_sema;
	submit_info.pWaitDstStageMask = wait_stages;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = buffers;

	VkSemaphore signal[] = { finished_semaphores[current_frame] };
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = signal;

	vkResetFences(dev, 1, &in_flight_fences[current_frame]);

	cassert(
		vkQueueSubmit(
			vgpu_t::ref().graphics_queue,
			1,
			&submit_info,
			in_flight_fences[current_frame]
		) == VK_SUCCESS,
		"failed to submit command buffer!\n"
	);

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal;

	VkSwapchainKHR swapchains[] = { new_swap };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swapchains;
	present_info.pImageIndices = img_idx;

	VkResult result = vkQueuePresentKHR(
		vgpu_t::ref().present_queue,
		&present_info
	);

	current_frame = (current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
	return result;
}

VkResult swap_t::acquire_next(u32* img_idx) {
	vkWaitForFences(
		vgpu_t::ref().handle,
		1,
		&in_flight_fences[current_frame],
		VK_TRUE,
		UINT64_MAX
	);

	VkResult result = vkAcquireNextImageKHR(
		vgpu_t::ref().handle,
		new_swap,
		UINT64_MAX,
		available_semaphores[current_frame],
		VK_NULL_HANDLE,
		img_idx
	);

	return result;
}

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
		framebuffer_count = 0;
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
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D swap_t::get_extent() {
	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		gpu_t::ref().handle,
		window_t::main().surface,
		&capabilities
	);
	
	VkExtent2D extent = {};
	extent.width	= window_t::main().width();
	extent.height	= window_t::main().height();

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
	info.surface = window_t::main().surface;

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

	VkAttachmentDescription da{};
	da.format = vkimage_t::depth_format();
	da.samples = VK_SAMPLE_COUNT_1_BIT;
	da.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	da.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	da.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	da.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	da.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	da.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference dar{};
	dar.attachment = 1;
	dar.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription ca = {};
	ca.format = vkimage_t::swap_format().format;
	ca.samples = VK_SAMPLE_COUNT_1_BIT;
	ca.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	ca.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	ca.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	ca.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	ca.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ca.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference car = {};
	car.attachment = 0;
	car.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &car;
	subpass.pDepthStencilAttachment = &dar;

	VkSubpassDependency dependency = {};
	dependency.dstSubpass = 0;
	dependency.dstAccessMask =
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | 
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.srcAccessMask = 0;
	dependency.srcStageMask =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

	VkAttachmentDescription attachments[2] = {ca, da};
	VkRenderPassCreateInfo rpi = {};
	rpi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpi.attachmentCount = 2;
	rpi.pAttachments = attachments;
	rpi.subpassCount = 1;
	rpi.pSubpasses = &subpass;
	rpi.dependencyCount = 1;
	rpi.pDependencies = &dependency;

	cassert(
		vkCreateRenderPass(
			vgpu_t::ref().handle,
			&rpi,
			NULL,
			&renderpass
		) == VK_SUCCESS,
		"swap_t failed to create renderpass!\n");
}

void swap_t::create_depth() {
	destroy_depth();
	depth_images = new vkimage_t[images_count];
	VkFormat depth_format = vkimage_t::depth_format();

	for (u32 i = 0; i < images_count; i++) {
		VkImageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.extent.width = extent.width;
		info.extent.height = extent.height;
		info.extent.depth = 1;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.format = depth_format;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.flags = 0;

		depth_images[i].create_from_info(
			info,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		

		VkImageViewCreateInfo view_info{};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = depth_images[i].img;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = depth_format;
		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		cassert(
			vkCreateImageView(
				vgpu_t::ref().handle,
				&view_info,
				nullptr,
				&depth_images[i].view
			) == VK_SUCCESS,
			"swap_t failed to create depth images!\n"
		);
	}
}

void swap_t::create_framebuffers() {
	destroy_framebuffers();

	framebuffers = new VkFramebuffer[images_count];
	framebuffer_count = images_count;

	for (u32 i = 0; i < images_count; i++) {
		
		VkImageView attachments[2] = { 
			images[i].view,
			depth_images[i].view
		};


		VkExtent2D se = extent;
		VkFramebufferCreateInfo fi = {};
		fi.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fi.renderPass = renderpass;
		fi.attachmentCount = 2;
		fi.pAttachments = attachments;
		fi.width = se.width;
		fi.height = se.height;
		fi.layers = 1;

		cassert(
			vkCreateFramebuffer(
				vgpu_t::ref().handle,
				&fi,
				nullptr,
				&framebuffers[i]
			) == VK_SUCCESS,
			"swap_t failed to create framebuffer!\n"
		);
		
	}
}

void swap_t::create_sync_objects() {
	destroy_sync_objects();
	images_in_flight = new VkFence[images_count];

	memset(images_in_flight, NULL, sizeof(VkFence) * images_count);

	VkSemaphoreCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		cassert(
			vkCreateSemaphore(
				vgpu_t::ref().handle,
				&info,
				nullptr,
				&available_semaphores[i]
			) == VK_SUCCESS,
			"swap_t failed to create semaphores\n"
		);

		cassert(
			vkCreateSemaphore(
				vgpu_t::ref().handle,
				&info,
				nullptr,
				&finished_semaphores[i]) == VK_SUCCESS,
				"swap_t failed to create finished semaphores\n"
		);

		cassert(
			vkCreateFence(
				vgpu_t::ref().handle,
				&fence_info,
				nullptr,
				&in_flight_fences[i]
			) == VK_SUCCESS,
			"swap_t failed to create in flight fences!\n"
		);
	}
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

swap_t::swap_t(){
	current_frame = 0;
	create();
}

swap_t::~swap_t() {
	destroy();
}