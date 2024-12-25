#ifndef GFX_SHADING_H
#define GFX_SHADING_H

#include "gfx/core/gfxctx.h"
#include "cmn/cmn.h"

typedef enum {
    GFX_SHADER_STAGE_FRAGMENT,
    GFX_SHADER_STAGE_VERTEX
} GFX_SHADER_STAGE;

typedef enum {
    GFX_SHADER_BUFFER_TYPE_STATIC,
    GFX_SHADER_BUFFER_TYPE_DYNAMIC,
    GFX_SHADER_BUFFER_TYPE_SAMPLER
} GFX_SHADER_BUFFER_TYPE;

typedef struct {
    VkViewport  viewport;
    VkRect2D    scissor;
} gfx_viewport_info_t;

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

    VkVertexInputBindingDescription*        binding_descriptions;
    uint32_t                                binding_description_count;
    VkVertexInputAttributeDescription*      attribute_descriptions;
    uint32_t                                attribute_description_count;

    VkDynamicState*                         dynamic_states;
    uint32_t                                dynamic_state_count;
} gfx_pipeline_info_t;

typedef struct {
    VkDescriptorSetLayoutBinding	binding;
    GFX_SHADER_BUFFER_TYPE			type;
    GFX_SHADER_STAGE                stage;
    uint32_t                        descriptor_count;
} gfx_shader_resource_t;

typedef struct {
    uint32_t                    resource_count;
    gfx_shader_resource_t*      resources;
    VkDescriptorSetLayout       set_layout;
} gfx_shader_layout_t;

typedef struct {
    VkPipelineLayout            pipeline_layout;
    gfx_shader_layout_t*        shader_layouts;
    uint32_t                    shader_layout_count;
} gfx_pipeline_layout_t;

typedef struct {
	uint32_t*	vert_bin;
	uint32_t*	frag_bin;
	uint32_t	vert_bin_len;
	uint32_t	frag_bin_len;

    gfx_pipeline_info_t pipeline_info;
    gfx_viewport_info_t viewport_info;
} gfx_shader_info_t;

typedef struct {
	VkShaderModule      frag_module;
	VkShaderModule      vert_module;
	VkPipeline	        pipeline;
    VkDescriptorPool    pool;
} gfx_shader_t;

void gfx_shader_new(gfx_shader_info_t* info, gfx_shader_t* out);
void gfx_shader_del(gfx_shader_t* in);
void gfx_shader_load_defaults();
void gfx_shader_unload_defaults();

void gfx_shader_layout_new(gfx_shader_layout_t* layout);
void gfx_shader_layout_del(gfx_shader_layout_t* layout);

void gfx_pipe_layout_new(gfx_pipeline_layout_t* layout);
void gfx_pipe_layout_del(gfx_pipeline_layout_t* layout);

void gfx_desc_pool_del(VkDescriptorPool pool);
void gfx_desc_pool_new(
    gfx_shader_layout_t* layouts,
    uint32_t layout_count,
    uint32_t max_sets,
    VkDescriptorPool* pool
);

void  gfx_allocate_descriptor_sets(
    VkDescriptorPool pool, 
    gfx_shader_layout_t* layouts,
    uint32_t layout_count, 
    VkDescriptorSet* descriptor_sets
);

void gfx_default_pipeline_info(gfx_pipeline_info_t* config_info);
void gfx_enable_alpha_blending(gfx_pipeline_info_t* config_info);

void gfx_pipe_add(
    gfx_shader_t* in,
    const gfx_pipeline_layout_t*    layout,
    const gfx_pipeline_info_t*      pinfo,
    const gfx_viewport_info_t*      vinfo,
    const win_t*                    window
);

void gfx_pipe_del(gfx_shader_t* in);


size_t gfx_compile_shader(
    char* src,
    size_t size,
    shaderc_shader_kind kind,
    char* out,
    const char* filename

);

#endif // !GFX_SHADING_H
