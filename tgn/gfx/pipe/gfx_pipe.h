#ifndef GFX_PIPE_H
#define GFX_PIPE_H
#include "gfx/gfx.h"
#include <inttypes.h>

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
    win_t*              window;
} gfx_pipe_info_t;

typedef struct {
    VkPipeline          pipeline;
    VkPipelineLayout    layout;
} gfx_pipe_t;



void gfx_pipe_def(gfx_pipe_setting_t* settings);
void gfx_shader_new(const uint32_t* bin, size_t bin_size, VkShaderModule* mod);
void gfx_pipe_new(gfx_pipe_info_t* info, gfx_pipe_t* out);



size_t gfx_compile_shader(
    char* src,
    size_t size,
    shaderc_shader_kind kind,
    char* out,
    const char* filename

);

#endif // !GFX_PIPE_H
