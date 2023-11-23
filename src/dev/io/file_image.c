#include "backend/stb_image.h"
#include "io.h"

#define STB_IMAGE_IMPLEMENTATION
#include "backend/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "backend/stb_image_write.h"


void file_image_open(cstr_t path, file_img_t* image) {
    image->data = stbi_load(path, &image->width, &image->height, &image->channels, 0);
    validate(image->data != NULL, "failed to read file %s\n", path);
}

void file_image_close(file_img_t* image) {
    if (!image) return;
    stbi_image_free(image->data);
    memset(image, 0, sizeof(file_img_t));
}

void file_image_write(cstr_t path, file_img_t* image) {
    int success = stbi_write_png(
        path,
        image->width,
        image->height,
        image->channels,
        image->data,
        image->width * image->channels
    );
    validate(success, "failed to write image!\n");
}

void file_build_map_grid(cstr_t file_path, cstr_t name, cstr_t out_path) {

}