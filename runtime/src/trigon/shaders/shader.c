#include "shader.h"
#include <vulkan/vulkan.h>

/* ----------------------------- DEFINES ----------------------------- */

typedef struct {
	VkDescriptorPool		pool;
	VkDescriptorSet			set;
	VkDescriptorSetLayout	set_layout;
} vkshader_t;




uint32_t 
shader_new(const shader_info_t* info) {

}

uint32_t 
shader_add_resource(uint32_t shaderid, shader_binding_t binding) {

}

void	 
shader_compile(uint32_t id) {

}

void	 
shader_del(uint32_t id) {

}

