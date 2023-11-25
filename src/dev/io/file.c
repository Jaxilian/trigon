#include "io.h"
#include "dev/mem/mem.h"
#include <stdio.h>
#include <stdlib.h>

static char* file_buffer = NULL;

void file_close() {
    mem_free(file_buffer);
    file_buffer = NULL;
}

char* file_open(cstr path, size_t* out_size) {
    if (file_buffer) file_close();

    FILE* file = fopen(path, "rb");
    validate(file, "failed to open file %s!\n", path);

    size_t size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    file_buffer = (char*)mem_alloc(size);
    if (file_buffer == NULL) {
        fclose(file);
        return NULL;
    }

    fread(file_buffer, 1, size, file);
    fclose(file);

    if (out_size) {
        *out_size = size;
    }

    return file_buffer;
}