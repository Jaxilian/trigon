#include "trigon.h"


void tgn_upd_test(shader_t* shader) {
	VkCommandBuffer buff = gfx_cmdbuff(shader->win);
	vkCmdBindPipeline(buff, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->pipe.pipeline);
	vkCmdDraw(buff, 3, 1, 0, 0);
	
}