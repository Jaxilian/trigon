#pragma once
#include <inttypes.h>

typedef struct {
	uint32_t pos[3];
	uint32_t nor[3];
	uint32_t uv[2];
	uint32_t col[4];
} vertex_t; 

class mesh_t {
public:
	uint32_t  index_count	= 0;
	uint32_t  vertex_count	= 0;

	vertex_t* vertices		= nullptr;
	uint32_t* indices		= nullptr;

	mesh_t() {

	}

	~mesh_t() {
		if (vertices) {
			delete[] vertices;

			vertices		= nullptr;
			vertex_count	= 0;
		}

		if (indices) {
			delete[] indices;
			indices = nullptr;
			index_count = 0;
		}
	}
};