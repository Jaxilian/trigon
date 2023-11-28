#include "examples.h"
#include "dev/io/io.h"
#include "assets/shaders/shader.h"
#include "string.h"


static void file_finders() {

}

void run_examples() {
	file_finders();

	shader_info_t info = { 0 };
	info.shader_name = "vcolor";

	info.property_sets_count = 2;
	info.property_sets[0] = (shader_property_t){
		.properties_count = 1,
		.properties[1] = (shader_property_value_t){
			.binding = 0,
			.count = 1
		}
	};

	shader_new(&info);
}