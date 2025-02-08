#ifndef GFX_PIPE_H
#define GFX_PIPE_H

#include "gfx/gfx.h"
#include <inttypes.h>

#define MAX_DESC_PER_SET       8
#define MAX_DESC_SETS_IN_USE   4

typedef struct {
    VkPipelineViewportStateCreateInfo       viewport_info;
    VkPipelineInputAssemblyStateCreateInfo  input_assembly_info;
    VkPipelineRasterizationStateCreateInfo  rasterization_info;
    VkPipelineMultisampleStateCreateInfo    multisample_info;
    VkPipelineColorBlendAttachmentState     color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo     color_blend_info;
    VkPipelineDepthStencilStateCreateInfo   depth_stencil_info;
    VkPipelineDynamicStateCreateInfo        dynamic_state_info;
    VkPipelineLayout                        pipeline_layout;
    uint32_t                                subpass;
} gfx_pipe_setting_t;

typedef struct {
    gfx_pipe_setting_t  settings;
    VkShaderModule      fragmod;
    VkShaderModule      vertmod;
    win_t* window;
} gfx_pipe_info_t;

typedef struct {
    VkPipeline          pipeline;
    VkPipelineLayout    layout;
    VkDescriptorPool    descriptor_pool;  // <--- now owned by the pipeline
} gfx_pipe_t;

typedef enum {
    SHADER_STAGE_FRAGMENT,
    SHADER_STAGE_VERTEX,
    SHADER_STAGE_COUNT
} SHADER_STAGE;

typedef enum {
    SHADER_BUFFER_TYPE_STATIC,
    SHADER_BUFFER_TYPE_DYNAMIC,
    SHADER_BUFFER_TYPE_SAMPLER,
    SHADER_BUFFER_TYPE_COUNT
} SHADER_BUFFER_TYPE;

typedef struct {
    uint32_t            location;
    uint32_t            count;
    SHADER_BUFFER_TYPE  buffer_type;

    // internal usage
    VkDescriptorSetLayoutBinding _binding;
} shader_res_t;

typedef struct {
    uint32_t            resource_count;
    shader_res_t        resources[MAX_DESC_PER_SET];

    // internal usage
    VkDescriptorSet     _set;
    VkDescriptorSetLayout _layout;
} shader_res_set_t;

typedef struct {
    const char* pack;
    const char* name;
    win_t* window;
    uint32_t            set_count;
    shader_res_set_t    sets[MAX_DESC_SETS_IN_USE];
} shader_info_t;

void gfx_pipe_default(gfx_pipe_setting_t* settings);
void gfx_shader_new(const uint32_t* bin, size_t bin_size, VkShaderModule* mod);
void gfx_pipe_new(gfx_pipe_info_t* info, gfx_pipe_t* out);

void gfx_descriptor_setup(shader_res_t* res);

// Pipeline now controls descriptor pool creation
void gfx_desc_pool_setup(gfx_pipe_t* pipe, shader_res_set_t sets[MAX_DESC_SETS_IN_USE], uint32_t set_count);
void gfx_desc_pool_alloc(gfx_pipe_t* pipe, shader_res_set_t sets[MAX_DESC_SETS_IN_USE], uint32_t set_count);
void gfx_desc_layout_setup(shader_res_set_t* set);



void gfx_pipe_layout_setup(
    shader_res_set_t sets[MAX_DESC_SETS_IN_USE],
    uint32_t sets_count,
    VkPipelineLayout* out_layout
);

#endif // GFX_PIPE_H
