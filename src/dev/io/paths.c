#include "io.h"
#include <string.h>
#include <stdlib.h>

static char root_path[64]   = { 0 };
static char cursor[255]     = { 0 };

static void split_path(cstr str, cstr delimiter, cstr output[32], uint32_t* count) {
    *count = 0;  
    cstr last_non_delim = str;

    while (*str != '\0') {
        if (strchr(delimiter, *str) != NULL) {
            if (last_non_delim != str) {
                output[(*count)++] = last_non_delim;
                if (str != last_non_delim) {
                    *((char*)str) = '\0';
                }
            }
            last_non_delim = str + 1;
        }
        str++;
    }
    if (last_non_delim != str) {
        output[(*count)++] = last_non_delim;
    }
}

void path_new_root(cstr path) {

    cstr parts[32];
    uint32_t count = 0; 
    split_path(path, "\\", parts, &count);

    uint32_t index = 0;
	for (uint32_t i = 0; i < count-1; i++) {
        size_t len = strlen(parts[i]);
        for (size_t j = 0; j < len; j++) {
            root_path[index] = parts[i][j];
            index++;
        }
        root_path[index] = '\\';
        index++;
	}
}

cstr path_get_root() {
    return root_path;
}

cstr path_get_path(cstr path) {

    validate(strlen(root_path) + strlen(path) < 255,
        "max cursor len reached [255] in paths.c, you're using: %zu", strlen(root_path) + strlen(path)
    );

    memset(cursor, 0, 255);
    strcpy(cursor, root_path);
    strcat(cursor, path);
    strcat(cursor, "\\");
    return cursor;
}

cstr path_get_file(cstr path, cstr name) {
    char buffer[255] = { 0 };
    strcpy(buffer, path);

    validate(strlen(buffer) + strlen(path) < 255,
        "max cursor len reached [255] in paths.c, you're using: %zu", strlen(buffer) + strlen(path)
    );

    memset(cursor, 0, 255);
    strcpy(cursor, buffer);
    strcat(cursor, name);
    return cursor;
}

