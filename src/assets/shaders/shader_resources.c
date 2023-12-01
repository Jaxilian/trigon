#include "shader.h"
#include "dev/gpu/vk.h"

static vk_descriptor_set_t* desc_set_next(uint32_t* out_id) {
	for (uint32_t i = 0; i < MAX_SHADER_SETS; i++) {
		if (ctx->descriptor_sets[i].initialized) continue;
		if (out_id) *out_id = i;
		return &ctx->descriptor_sets[i];
	};
	validate(false, "max descriptor sets reached!\n %d", MAX_SHADER_SETS);
	return NULL;
}

static vk_descriptor_t* get_next(uint32_t* out_id) {
	for (uint32_t i = 0; i < MAX_UNIQUE_DESC; i++) {
		if (ctx->descriptors[i].initialized) continue;
		if (out_id) *out_id = i;
		return &ctx->descriptors[i];
	};
	validate(false, "max descriptor cache reached!\n %d", MAX_UNIQUE_DESC);
	return NULL;
}

void shader_resource_new(shader_resource_info_t* in, shader_resource_t* out) {
	memset(out, 0, sizeof(shader_resource_t));
	uint32_t id = 0;
	vk_descriptor_t* desc = get_next(&id);
	out->desc_id = id;
	out->count = in->count;
	out->binding = in->binding;
	out->type = in->type;
	out->data_count = in->data_count;
	out->data_stride = in->data_stride;
	vk_descriptor_new(in->binding, in->count, in->type, desc);
}

void shader_resource_del(shader_resource_t* in) {
	memset(&ctx->descriptors[in->desc_id], 0, sizeof(vk_descriptor_t));
}

static vk_buffer_t* get_next_buffer() {
	for (uint32_t i = 0; i < MAX_SHADER_BUFFERS; i++) {
		if (ctx->descriptor_buffers[i].initialized) continue;
		return &ctx->descriptor_buffers[i];
	};
	validate(false, "max shader buffers reached!\n %d", MAX_SHADER_BUFFERS);
	return NULL;
}

void shader_resource_group_bind_buffers(shader_resource_group_t* in) {
	VkDescriptorBufferInfo buffer_infos[MAX_SHADER_BUFFERS];
	VkWriteDescriptorSet descritpro_writes[MAX_SHADER_BUFFERS];
	vk_descriptor_set_t* target = &ctx->descriptor_sets[in->desc_set_id];
	validate(target->initialized, "descriptor set was NULL, while trying to set data to it!\n");

	for (uint32_t i = 0; i < in->resource_count; i++) {
		vk_buffer_t* buffer = get_next_buffer();
		validate(buffer != NULL, "vk_buffer_t was already initialized while trying to set data to the resource group!\n");
		vk_buffer_new(in->resources[i].data_stride, (uint32_t)in->resources[i].data_count, buffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		
		buffer_infos[i] = (VkDescriptorBufferInfo){
			.buffer = buffer->buffer,
			.offset = 0,
			.range = buffer->size
		};

		descritpro_writes[i] = (VkWriteDescriptorSet){
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = target->set,
			.dstBinding = in->resources[i].binding,
			.descriptorCount = in->resources[i].count,
			.descriptorType = vk_descriptor_convert_type(in->resources[i].type),
			.pBufferInfo = &buffer_infos[i],
		};

		in->resources[i].data = buffer;
	}

	vkUpdateDescriptorSets(ctx->device.device, in->resource_count, descritpro_writes, 0, NULL);
}

void shader_resource_group_new(shader_resource_group_info_t* info, shader_resource_group_t* out) {
	memset(out, 0, sizeof(shader_resource_group_t));

	out->resource_count = info->resource_count;
	
	vk_descriptor_t descriptors[MAX_DESC_PER_SET] = { 0 };
	for (uint32_t i = 0; i < out->resource_count; i++) {
		if (info->resources[i].type == SHADER_DYNAMIC_BUFFER) {
			out->has_dynamic_buffer = true;
		}

		memcpy(
			&out->resources[i],
			&info->resources[i],
			sizeof(shader_resource_t)
		);

		vk_descriptor_t* target = &ctx->descriptors[i];
		validate(target->initialized, "trying to create a new shader resource group, but vk_descriptor_t was NULL!\n");
		memcpy(&descriptors[i], target, sizeof(vk_descriptor_t));
		out->resources[i].data_stride = info->resources[i].data_stride;
		out->resources[i].data_count = info->resources[i].data_count;
	}

	vk_descriptor_set_t* set = desc_set_next(&out->desc_set_id);
	vk_descriptor_set_new(descriptors, out->resource_count, set);
	shader_resource_group_bind_buffers(out);

	memset(ctx->descriptors, 0, sizeof(vk_descriptor_t) * MAX_UNIQUE_DESC);
}

void shader_resource_group_del(shader_resource_group_t* out) {
	vk_descriptor_set_t* target = &ctx->descriptor_sets[out->desc_set_id];
	if (!target->initialized) return;

	for (uint32_t i = 0; i < out->resource_count; i++) {
		vk_buffer_del((vk_buffer_t*) out->resources[i].data);
	}

	vk_descriptor_set_del(target);
	memset(out, 0, sizeof(shader_resource_group_t));
}

void shader_resource_set(shader_resource_t* resource, void* data) {
	vk_buffer_set(resource->data, data);
}
