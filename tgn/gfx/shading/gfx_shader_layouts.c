#include "gfx_shading.h"
#include <stdlib.h>
#include "cmn/cmn.h"
#include <memory.h>

void gfx_shader_layout_new(gfx_shader_layout_t* layout) {
	VkDevice dev = gfx_dev()->device;
	if (!dev)return;

	VkDescriptorSetLayoutBinding* bindings = malloc(
		sizeof(VkDescriptorSetLayoutBinding) * layout->resource_count
	);

	if (!bindings) {
		debug_err("failed to allocate memory for desc set layout!\n");
		return;
	}

    for (uint32_t i = 0; i < layout->resource_count; ++i) {
        memcpy(&bindings[i].binding, &layout->resources[i].binding, sizeof(VkDescriptorSetLayoutBinding));
        bindings[i].descriptorCount = layout->resources[i].descriptor_count;

        switch (layout->resources[i].type) {
        case GFX_SHADER_BUFFER_TYPE_DYNAMIC:
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            break;
        case GFX_SHADER_BUFFER_TYPE_STATIC:
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            break;
        case GFX_SHADER_BUFFER_TYPE_SAMPLER:
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            break;
        default:
            free(bindings);
            debug_err("Unkown GFX_SHADER_TYPE\n");
            return;
        }

        switch (layout->resources[i].stage) {
        case GFX_SHADER_STAGE_VERTEX:
            bindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case GFX_SHADER_STAGE_FRAGMENT:
            bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        default:
            debug_err("Unkown GFX_SHADER_STAGE\n");
            break;
        }


        bindings[i].pImmutableSamplers = NULL; // Set if using immutable samplers
    }

    VkDescriptorSetLayoutCreateInfo layout_info = {0};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = layout->resource_count;
    layout_info.pBindings = bindings;

    VkResult result = vkCreateDescriptorSetLayout(dev, &layout_info, NULL, &layout->set_layout);
    free(bindings);
}

void gfx_shader_layout_del(gfx_shader_layout_t* layout) {
    VkDevice dev = gfx_dev()->device;
    if (dev && layout && layout->set_layout) {
        vkDestroyDescriptorSetLayout(dev, layout->set_layout, NULL);
        layout->set_layout = VK_NULL_HANDLE;
    }
}

void gfx_pipe_layout_new(gfx_pipeline_layout_t* layout) {
    VkDevice dev = gfx_dev()->device;

    VkDescriptorSetLayout* set_layouts = malloc(
        sizeof(VkDescriptorSetLayout) * layout->shader_layout_count
    );

    if (!set_layouts) {
        debug_err("failed to allocate memory for set_layouts in pipe_layout_new\n");
        return;
    }

    for (uint32_t i = 0; i < layout->shader_layout_count; ++i) {
        set_layouts[i] = layout->shader_layouts[i].set_layout;
    }

    VkPipelineLayoutCreateInfo pipeline_layout_info = {0};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = layout->shader_layout_count;
    pipeline_layout_info.pSetLayouts = set_layouts;

    VkResult result = vkCreatePipelineLayout(
        dev,
        &pipeline_layout_info,
        NULL,
        &layout->pipeline_layout
    );

    free(set_layouts);
}

void gfx_pipe_layout_del(gfx_pipeline_layout_t* layout) {
    VkDevice dev = gfx_dev()->device;

    if (dev && layout && layout->pipeline_layout) {
        vkDestroyPipelineLayout(dev, layout->pipeline_layout, NULL);
        layout->pipeline_layout = VK_NULL_HANDLE;
    }
}

void gfx_desc_pool_new(
    gfx_shader_layout_t* layouts,
    uint32_t layout_count,
    uint32_t max_sets,
    VkDescriptorPool* pool
) {
    VkDevice dev = gfx_dev()->device;

    uint32_t uniform_buffer_count = 0;
    uint32_t combined_image_sampler_count = 0;
    uint32_t storage_buffer_count = 0;

    for (uint32_t i = 0; i < layout_count; ++i) {
        for (uint32_t j = 0; j < layouts[i].resource_count; ++j) {
            GFX_SHADER_BUFFER_TYPE type = layouts[i].resources[j].type;
            switch (type) {
                case GFX_SHADER_BUFFER_TYPE_DYNAMIC:
                    uniform_buffer_count += layouts[i].resources[j].descriptor_count * max_sets;
                    break;
                case GFX_SHADER_BUFFER_TYPE_STATIC:
                   storage_buffer_count += layouts[i].resources[j].descriptor_count * max_sets;
                    break;
                case GFX_SHADER_BUFFER_TYPE_SAMPLER:
                    combined_image_sampler_count += layouts[i].resources[j].descriptor_count * max_sets;
                default:
                    return;
            }
        }
    }

    uint32_t pool_size_count = 0;
    VkDescriptorPoolSize pool_sizes[2]; 

    if (uniform_buffer_count > 0) {
        pool_sizes[pool_size_count].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[pool_size_count].descriptorCount = uniform_buffer_count;
        pool_size_count++;
    }

    if (combined_image_sampler_count > 0) {
        pool_sizes[pool_size_count].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_sizes[pool_size_count].descriptorCount = combined_image_sampler_count;
        pool_size_count++;
    }

    VkDescriptorPoolCreateInfo pool_info = {0};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_size_count;
    pool_info.pPoolSizes = pool_sizes;
    pool_info.maxSets = max_sets * layout_count;

    vkCreateDescriptorPool(dev, &pool_info, NULL, pool);
}

void gfx_desc_pool_del(VkDescriptorPool pool) {
    VkDevice dev = gfx_dev()->device;
    if (dev && pool) {
        vkDestroyDescriptorPool(dev, pool, NULL);
    }
}

void  gfx_allocate_descriptor_sets(
    VkDescriptorPool pool,
    gfx_shader_layout_t* layouts,
    uint32_t layout_count,
    VkDescriptorSet* descriptor_sets
) {
    VkDevice dev = gfx_dev()->device;

    if (!dev || !pool || !layouts || !descriptor_sets) {
        debug_err("argument missing!\n");
        return;
    }

    VkDescriptorSetLayout* set_layouts = malloc(sizeof(VkDescriptorSetLayout) * layout_count);
    if (!set_layouts) {
        debug_err("failed to allocate memory for set_layout in alloc_descriptor_sets\n");
        return;
    }

    for (uint32_t i = 0; i < layout_count; ++i) {
        set_layouts[i] = layouts[i].set_layout;
    }

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.descriptorSetCount = layout_count;
    alloc_info.pSetLayouts = set_layouts;

    VkResult result = vkAllocateDescriptorSets(dev, &alloc_info, descriptor_sets);
    free(set_layouts);
}