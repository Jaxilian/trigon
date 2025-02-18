#include "tgn_data.h"
#include <gfx/core/gfxctx.h>
#include <memory.h>

typedef struct {
    VkBuffer                buffer;
    VkDeviceMemory          memory;
    VkDescriptorSet         set;
    void*                   mapped;
    
} scene_framebuffer_t;

static  scene_framebuffer_t      buffers[MAX_FRAMES_IN_FLIGHT] = { 0 };
static  VkDescriptorSetLayout    layout;
static  VkDescriptorPool        descriptor_pool;

void tgn_scene_data_init() {

    vkdev_t* pdev = gfx_dev();
    VkDeviceSize buffsize = sizeof(scene_data_t);

    // framebuffers 

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        VkBufferCreateInfo binf = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        binf.size = buffsize;
        binf.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        vkCreateBuffer(pdev->device, &binf, NULL, &buffers[i].buffer);

        VkMemoryRequirements memreq;
        vkGetBufferMemoryRequirements(pdev->device, buffers[i].buffer, &memreq);

        VkMemoryAllocateInfo ainf = {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO 
        };

        ainf.allocationSize = memreq.size;
        ainf.memoryTypeIndex = gfx_find_memory_type(
            pdev->physical, memreq.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        vkAllocateMemory(pdev->device, &ainf, NULL, &buffers[i].memory);
        vkBindBufferMemory(pdev->device, buffers[i].buffer, buffers[i].memory, 0);
        
        vkMapMemory(pdev->device, buffers[i].memory, 0, buffsize, 0, &buffers[i].mapped);
    }

    // descriptor set layout

    VkDescriptorSetLayoutBinding binding = {0};
    binding.binding = 0; // Binding index in shader
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layout_info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    layout_info.bindingCount = 1;
    layout_info.pBindings = &binding;


    vkCreateDescriptorSetLayout(pdev->device, &layout_info, NULL, &layout);


    // allocate descriptor set

    VkDescriptorPoolSize poolsize = {0};
    poolsize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolsize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

    VkDescriptorPoolCreateInfo pinf = {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
      .maxSets = MAX_FRAMES_IN_FLIGHT,
      .poolSizeCount = 1,
      .pPoolSizes = &poolsize
    };
 
    vkCreateDescriptorPool(pdev->device, &pinf, NULL, &descriptor_pool);

    VkDescriptorSetAllocateInfo ainf = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    ainf.descriptorPool = descriptor_pool;
    ainf.descriptorSetCount = 1;
    ainf.pSetLayouts = &layout;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkAllocateDescriptorSets(pdev->device, &ainf, &buffers[i].set);

        VkDescriptorBufferInfo binf = {0};
        binf.buffer = buffers[i].buffer;
        binf.offset = 0;
        binf.range = sizeof(scene_data_t);

        VkWriteDescriptorSet descwrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descwrite.dstSet = buffers[i].set;
        descwrite.dstBinding = 0;
        descwrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descwrite.descriptorCount = 1;
        descwrite.pBufferInfo = &binf;

        vkUpdateDescriptorSets(pdev->device, 1, &descwrite, 0, NULL);
    }

}


void tgn_scene_data_update(const scene_data_t* data, const win_t* win) {  
    memcpy(buffers[win->current_frame].mapped, data, sizeof(scene_data_t));
}

void tgn_scene_data_bind(VkCommandBuffer cmd, VkPipelineLayout layout, const win_t* win) {
    vkCmdBindDescriptorSets(
        cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout,
        0, 1, &buffers[win->current_frame].set, 0, NULL
    );
}

void tgn_scene_data_cleanup() {
    VkDevice device = gfx_dev()->device;
    vkDestroyDescriptorSetLayout(device, layout, NULL);
    vkDestroyDescriptorPool(device, descriptor_pool, NULL);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkUnmapMemory(device, buffers[i].memory);
        vkDestroyBuffer(device, buffers[i].buffer, NULL);
        vkFreeMemory(device, buffers[i].memory, NULL);
    }
}