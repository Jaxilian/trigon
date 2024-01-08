#ifndef VSTD_H
#define VSTD_H
#include <stdint.h>

typedef struct {
	char	key[32];
	size_t	data_size;
	void*	data;
} cfg_pair_t;

typedef struct {
	size_t		file_size;
	uint32_t	pair_len;
	cfg_pair_t* pairs;
} cfg_data_t;

void cfg_load(const char* file_path, cfg_data_t* out);
void cfg_save(const char* file_path, const cfg_data_t* in);
void cfg_clear();

#endif // !VSTD_H
