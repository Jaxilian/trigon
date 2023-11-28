#ifndef VK_CMN_H
#define VK_CMN_H

#define MAX_DESCRIPTOR_SETS_IN_USE 4
#define MAX_SHADERS 16
#define MAX_SHADER_SETS 16
#define MAX_DESC_PER_SET 8

typedef enum {
	SHADER_STATIC_BUFFER,
	SHADER_DYNAMIC_BUFFER,
	SHADER_SAMPLER_BUFFER
} shader_property_e;

#endif // !VK_CMN_H
