#ifndef TRIGON_SHADER_H
#define TRIGON_SHADER_H
#include "gfx/gfx.h"
#include <inttypes.h>
#include "gfx/pipe/gfx_pipe.h"

#define MAX_DESC_PER_SET 8
#define MAX_DESC_SETS_IN_USE 4

typedef enum {
	SHADER_STAGE_FRAGMENT,
	SHADER_STAGE_VERTEX
} SHADER_STAGE;

typedef enum {
	SHADER_BUFFER_TYPE_STATIC,
	SHADER_BUFFER_TYPE_DYNAMIC,
	SHADER_BUFFER_TYPE_SAMPLER
} SHADER_BUFFER_TYPE;


typedef struct {
	uint32_t			location;
	uint32_t			count;
	SHADER_BUFFER_TYPE	buffer_type;

	// don't create, it's internal
	VkDescriptorSetLayoutBinding _binding;
} shader_res_t;

typedef struct {
	uint32_t				resource_count;
	shader_res_t			resources[MAX_DESC_PER_SET];

	// don't create, it's internal
	VkDescriptorPool		_pool;

	// don't create, it's internal
	VkDescriptorSet			_set;

	// don't create, it's internal
	VkDescriptorSetLayout	_layout;
} shader_res_set_t;

typedef struct {
	const char*			pack;
	const char*			name;
	win_t*				window;
	uint32_t			set_count;
	shader_res_set_t	sets[MAX_DESC_SETS_IN_USE];
} shader_info_t;

typedef struct {
	const char* pack;
	const char* name;
	gfx_pipe_t	pipe;
	win_t*		win;
} shader_t;

void shader_new(shader_t* shader, shader_info_t* info);
void shader_del(shader_t* shader);
void shaders_clear();


void tgn_pipe_layout_gen(
	shader_res_set_t sets[MAX_DESC_SETS_IN_USE],
	uint32_t sets_count,
	VkPipelineLayout* out
);

#endif // !SHADER_H
