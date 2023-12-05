#include "textures.h"

void texture_sample(texture_t* texture, vec2 position, vec4 color) {
	bool min_ok = position[0] >= 0 && position[1] >= 0;
	bool max_ok = position[0] < texture->width && position[1] < texture->height;
	validate(min_ok && max_ok, "poisition out of bounds!\n");

	int pos = (
		(uint32_t)position[1] * texture->width + (uint32_t)position[0]
		) * texture->channels;

	unsigned char r = (unsigned char)(texture->data[pos + 0]); // Red
	unsigned char g = (unsigned char)(texture->data[pos + 1]); // Green
	unsigned char b = (unsigned char)(texture->data[pos + 2]); // Blue
	unsigned char a = (unsigned char)(texture->channels >= 4 ? texture->data[pos + 3] : 255);

	glm_vec4_copy(
		(vec4) {
		(float)r / 255.0f,
			(float)g / 255.0f,
			(float)b / 255.0f,
			(float)a / 255.0f
	},
		color
	);
}