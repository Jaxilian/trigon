#include "vk.h"


static uint32_t get_next() {
	for (uint32_t i = 0; i < MAX_DESCRIPTOR_SETS; i++) {
		if (ctx->sets[i].initialized) continue;
		return i;
	}
	validate(false, "max descriptor set reached [%d]\n", MAX_DESCRIPTOR_SETS);
	return 0;
}

static vk_descriptor_t* get_desc(uint32_t id) {
	vk_descriptor_t* desc = &ctx->sets[id];
	if (!desc->initialized) {
		return NULL;
	}
	return desc;
}

uint32_t vk_descriptor_new(VkDescriptorSetLayoutBinding info[MAX_DESCRIPTORS_PER_SET], uint32_t count) {
	uint32_t id = get_next();
	vk_descriptor_t* desc = get_desc(id);
	if (desc->initialized) return id;

	VkDescriptorSetLayoutCreateInfo layout_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = count,
		.pBindings = info,
	};

	validate(vkCreateDescriptorSetLayout(
		ctx->device.device,
		&layout_info,
		NULL,
		&desc->set_layout) == VK_SUCCESS,
		"failed to create descriptor set layout!\n"
	);

	VkDescriptorPoolSize pool_sizes[MAX_DESCRIPTORS_PER_SET] = { 0 };
	for (uint8_t i = 0; i < count; i++) {
		pool_sizes[i].type = info->descriptorType;
		pool_sizes[i].descriptorCount = info->descriptorCount;
	}

	VkDescriptorPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.poolSizeCount = count,
		.pPoolSizes = pool_sizes,
		.maxSets = 1,
	};

	validate(vkCreateDescriptorPool(
		ctx->device.device,
		&pool_info,
		NULL,
		&desc->descriptor_pool) == VK_SUCCESS,
		"failed to create descriptor pool layout!\n"
	);

	VkDescriptorSetAllocateInfo alloc_info = {
	   .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
	   .descriptorPool = desc->descriptor_pool,
	   .descriptorSetCount = 1,
	   .pSetLayouts = &desc->set_layout,
	};

	validate(vkAllocateDescriptorSets(
		ctx->device.device,
		&alloc_info,
		&desc->descriptor_set) == VK_SUCCESS,
		"failed to create descriptor pool layout!\n"
	);

	desc->initialized = true;
	memcpy(desc->descriptors, info, sizeof(VkDescriptorSetLayoutBinding) * MAX_DESCRIPTORS_PER_SET);
	desc->descriptor_count = count;

	return id;

}

void vk_descriptor_del(uint32_t id) {
	vk_descriptor_t* desc = get_desc(id);
	if (!desc) return;

	vkDestroyDescriptorPool(ctx->device.device, desc->descriptor_pool, NULL);
	vkDestroyDescriptorSetLayout(ctx->device.device, desc->set_layout, NULL);
	memset(desc, 0, sizeof(vk_descriptor_t));
}

void vk_descriptors_bind(uint32_t descriptor, uint32_t pipeline_layout) {
	vk_descriptor_t* desc = get_desc(descriptor);
	if (!desc) return;

	uint32_t dynamic_offset = 0;
	uint32_t dynamic_offset_count = (desc->descriptor_count > 0) ? 1 : 0;
	uint32_t* p_dynamic_offsets = (desc->descriptor_count > 0) ? &dynamic_offset : NULL;

	vkCmdBindDescriptorSets(
		vk_command_buffer(),
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		ctx->pipeline_layouts[pipeline_layout],
		0, // First set; adjust if you're binding multiple sets
		1, // Number of sets to bind
		&desc->descriptor_set, // Use the descriptor set from the vk_descriptor_t struct
		dynamic_offset_count,
		p_dynamic_offsets
	);
}

static uint32_t get_next_pipeline_layout() {
	for (uint32_t i = 0; i < MAX_SHADERS; i++) {
		if (ctx->pipeline_layouts[i] != VK_NULL_HANDLE) continue;
		return i;
	}
	validate(false, "max pipeline layouts reached [%d]\n", MAX_SHADERS);
	return 0;
}

uint32_t vk_pipeline_layout_new(uint32_t descriptors[MAX_DESCRIPTOR_SETS_IN_USE], uint32_t count) {

	VkDescriptorSetLayout layouts[MAX_DESCRIPTOR_SETS_IN_USE];
	for (uint32_t i = 0; i < count; i++) {
		vk_descriptor_t* desc = get_desc(descriptors[i]);
		validate(desc->initialized, "descriptor %d was not initialized!\n", i);
		if (!desc->initialized) continue;

		layouts[i] = desc->set_layout;
	}

	VkPipelineLayoutCreateInfo pipeline_layout_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = count,
		.pSetLayouts = layouts
	};

	uint32_t id = get_next_pipeline_layout();
	validate(vkCreatePipelineLayout(
		ctx->device.device,
		&pipeline_layout_info,
		NULL,
		&ctx->pipeline_layouts[id])
		, "failed to create pipeline layout!\n"
	);
	return id;
}

void vk_pipeline_layout_del(uint32_t id) {

	vkDestroyPipelineLayout(ctx->device.device, ctx->pipeline_layouts[id], NULL);
	ctx->pipeline_layouts[id] = VK_NULL_HANDLE;
	
}

void vk_pipeline_layout_clear() {
	for (uint32_t i = 0; i < MAX_SHADERS; i++) {
		if (ctx->pipeline_layouts[i] == VK_NULL_HANDLE) continue;
		vk_pipeline_layout_del(i);
	}
}

void vk_descriptor_clear() {
	for (uint32_t i = 0; i < MAX_DESCRIPTOR_SETS; i++) {
		if (!ctx->sets[i].initialized) continue;
		vk_descriptor_del(i);
	}
}

void vk_descriptors_update(uint32_t descriptor_id, uint32_t binding_id, vk_buffer_t* buffers[MAX_DESCRIPTOR_SETS_IN_USE]) {
	vk_descriptor_t* desc = get_desc(descriptor_id);
	validate(desc != NULL, "descriptor with id %d was NULL!\n", descriptor_id);

	VkDescriptorBufferInfo buffer_infos[MAX_DESCRIPTORS_PER_SET];
	VkWriteDescriptorSet descriptor_writes[MAX_DESCRIPTORS_PER_SET];
	VkDescriptorSetLayoutBinding* binding = &desc->descriptors[binding_id];

	for (uint32_t i = 0; i < binding->descriptorCount; i++) {
		validate(buffers[i] != NULL, "Buffer at index %d is NULL!\n", i);
		vk_buffer_t* buffer = buffers[i];
		buffer_infos[i] = (VkDescriptorBufferInfo){
			.buffer = buffer->buffer,
			.offset = 0,
			.range = buffer->size
		};

		descriptor_writes[i] = (VkWriteDescriptorSet){
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = desc->descriptor_set,
			.dstBinding = binding_id, // Set to binding_id instead of i
			.descriptorCount = 1, // Each write updates one descriptor
			.descriptorType = binding->descriptorType,
			.pBufferInfo = &buffer_infos[i],
		};
	}

	vkUpdateDescriptorSets(ctx->device.device, binding->descriptorCount, descriptor_writes, 0, NULL);
}