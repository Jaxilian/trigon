#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include "cstr.h"
#include <wchar.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

static char* file_cache = NULL;

bool path_new(path_os_t* path, const char* path_str) {
    if (strlen(path_str) >= MAX_PATH) {
        fprintf(stderr, "provided path_str was larger than MAX_PATH");
        return false;
    }

    memset(path->data, 0, sizeof(path->data));
    strcpy(path->data, path_str);
    return true;
}

bool has_extension(const char* filename, const char* extension) {
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return false;
    }
    return strcmp(dot + 1, extension) == 0;
}

bool list_files_with_extension(
    const path_os_t* parent_path,
    const char* extension,
    int* count,
    path_os_t* paths) {

    if (parent_path == NULL || extension == NULL) {
        fprintf(stderr, "Invalid input\n");
        return false;
    }

#ifdef _WIN32
    WCHAR search_path[MAX_PATH];
    _snwprintf_s(search_path, MAX_PATH, _TRUNCATE, L"%S\\*", parent_path->data);

    WIN32_FIND_DATA find_file_data;
    HANDLE h_find = FindFirstFile(search_path, &find_file_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Find first file failed (%lu)\n", GetLastError());
        return false;
    }

    int index = 0;
    do {
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        char file_name[MAX_PATH];
        size_t converted_chars = 0;
        errno_t wcstombs_result = wcstombs_s(&converted_chars, file_name, MAX_PATH, find_file_data.cFileName, _TRUNCATE);

        if (wcstombs_result != 0) {
            continue;
        }

        if (!has_extension(file_name, extension)) {
            continue;
        }

        if (paths && index < *count) {
            WCHAR full_path_wide[MAX_PATH];
            _snwprintf_s(full_path_wide, MAX_PATH, _TRUNCATE, L"%S\\%s", parent_path->data, find_file_data.cFileName);

            size_t dest_size = sizeof(paths[index].data);
            size_t converted_chars = 0;
            errno_t conversion_result = wcstombs_s(&converted_chars, paths[index].data, dest_size, full_path_wide, _TRUNCATE);

            if (conversion_result != 0) {
                perror("Error in converting wide char to multibyte.\n");
                continue;
            }
        }

        index++;
    } while (FindNextFile(h_find, &find_file_data) != 0);

    *count = index;
    FindClose(h_find);
#else
    DIR* dp = opendir(parent_path->data);
    if (dp == NULL) {
        perror("opendir");
        return false;
    }

    struct dirent* entry;
    int index = 0;
    while ((entry = readdir(dp)) != NULL) {
    if (entry->d_type != DT_DIR && has_extension(entry->d_name, extension)) {
        if (paths && index < *count) {
            snprintf(paths[index].data, sizeof(paths[index].data), "%s/%s", parent_path->data, entry->d_name);
        }
        index++;
    }
}

    *count = index;
    closedir(dp);
#endif

    return true;
}

bool path_get_subfiles_filter_count(const path_os_t* path, int* out_count, const char* extension) {
    return list_files_with_extension(path, extension, out_count, NULL);
}

bool path_get_subfiles_filter(const path_os_t* path, path_os_t* files, int out_count, const char* extension) {
    return list_files_with_extension(path, extension, &out_count, files);
}

bool list_files(
    const path_os_t* parent_path,
    int* count,
    path_os_t* paths) {

    if (parent_path == NULL) {
        fprintf(stderr, "parent_path is NULL\n");
        return false;
    }

#ifdef _WIN32
    WCHAR search_path[MAX_PATH];
    _snwprintf_s(search_path, MAX_PATH, _TRUNCATE, L"%S\\*", parent_path->data);

    WIN32_FIND_DATA find_file_data;
    HANDLE h_find = FindFirstFile(search_path, &find_file_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "find child failed (%lu)\n", GetLastError());
        return false;
    }

    int index = 0;
    do {
        if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        if (paths && index < *count) {
            WCHAR full_path_wide[MAX_PATH];
            _snwprintf_s(full_path_wide, MAX_PATH, _TRUNCATE, L"%S\\%s", parent_path->data, find_file_data.cFileName);

            size_t dest_size = sizeof(paths[index].data);
            size_t converted_chars = 0;
            errno_t conversion_result = wcstombs_s(&converted_chars, paths[index].data, dest_size, full_path_wide, _TRUNCATE);

            if (conversion_result != 0) {
                perror("Error in converting wide char to multibyte.\n");
                continue;
            }
        }

        index++;
    } while (FindNextFile(h_find, &find_file_data) != 0);

    *count = index;
    FindClose(h_find);
#else
    DIR* dp = opendir(parent_path->data);
    if (dp == NULL) {
        perror("opendir");
        return false;
    }

    struct dirent* entry;
    int index = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_type != DT_DIR) { // If the entry is not a directory
            if (paths && index < *count) {
                snprintf(paths[index].data, sizeof(paths[index].data), "%s/%s", parent_path->data, entry->d_name);
            }
            index++;
        }
    }
    
    *count = index;
    closedir(dp);
#endif

    return true;
}

bool path_get_subfiles_count(const path_os_t* path, int* out_count) {
    return list_files(path, out_count, NULL);
}

bool path_get_subfiles(const path_os_t* path, path_os_t* files, int out_count) {
    return list_files(path, &out_count, files);
}

bool list_subdirectories(
    const path_os_t* parent_path,
    int* count,
    path_os_t* paths) {

    if (parent_path == NULL) {
        fprintf(stderr, "parent_path is NULL\n");
        return false;
    }

#ifdef _WIN32
    WCHAR search_path[MAX_PATH];
    _snwprintf_s(search_path, MAX_PATH, _TRUNCATE, L"%S\\*", parent_path->data); // Use _snwprintf_s with _TRUNCATE

    WIN32_FIND_DATA find_file_data;
    HANDLE h_find = FindFirstFile(search_path, &find_file_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "find child failed (%lu)\n", GetLastError());
        return false;
    }

    int index = 0;
    do {
        if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            continue;
        }

        if (wcscmp(find_file_data.cFileName, L".") != 0 && wcscmp(find_file_data.cFileName, L"..") != 0) {
            WCHAR full_path_wide[MAX_PATH];
            _snwprintf_s(full_path_wide, MAX_PATH, _TRUNCATE, L"%S\\%s", parent_path->data, find_file_data.cFileName);

            if (paths && index < *count) {
                size_t dest_size = sizeof(paths[index].data);
                size_t converted_chars = 0;
                errno_t conversion_result = wcstombs_s(&converted_chars, paths[index].data, dest_size, full_path_wide, _TRUNCATE);

                if (conversion_result != 0) {
                    perror("Error in converting wide char to multibyte.\n");
                    continue;
                }
            }

            index++;
        }

    } while (FindNextFile(h_find, &find_file_data) != 0);

    *count = index;

    FindClose(h_find);
#else

  DIR* dp = opendir(parent_path->data);
    if (dp == NULL) {
        perror("opendir");
        return false;
    }

    struct dirent* entry;
    int index = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            if (paths && index < *count) {
                snprintf(paths[index].data, sizeof(paths[index].data), "%s/%s", parent_path->data, entry->d_name);
            }
            index++;
        }
    }

    closedir(dp);
#endif

    *count = index;
    return true;
}

bool path_get_subdir_count(const path_os_t* dir, int* out) {
    return list_subdirectories(dir, out, NULL);
}

bool path_get_subdirs(const path_os_t* dir, path_os_t* out, int out_count) {
    return list_subdirectories(dir, &out_count, out);
}

bool path_get_current(path_os_t* path) {
    if (path == NULL) {
        fprintf(stderr, "[error] path is NULL\n");
        return false;
    }

#ifdef _WIN32
    WCHAR buff[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH, buff);
    wchar_to_cstr(buff, path->data);
#else
    getcwd(path->data, MAX_PATH);
#endif
        return true;
}

bool path_get_parent(const path_os_t* dir, path_os_t* parent) {
    if (dir == NULL || parent == NULL) {
        fprintf(stderr, "[error] input dir path is NULL\n");
        return false;
    }

    size_t dir_length = strlen(dir->data);
    if (dir_length == 0 || dir_length >= MAX_PATH) {
        fprintf(stderr, "[error] invalid dir path length\n");
        return false;
    }

    int last_sep_pos = -1;
    for (int i = 0; i < (int)dir_length; ++i) {
        if (dir->data[i] == '\\' || dir->data[i] == '/') {
            last_sep_pos = i;
        }
    }

    if (last_sep_pos == -1) {
        fprintf(stderr, "[error] no parent directory found\n");
        return false;
    }

    size_t copy_length = last_sep_pos == 0 ? 1 : last_sep_pos;
    memcpy(parent->data, dir->data, copy_length);
    parent->data[copy_length] = '\0';
    return true;
}

bool path_exists(const path_os_t* path) {
    if (path == NULL) {
        printf("path was null!\n");
        return false;
    }
 
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributesA(path->data);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES);
#else
    struct stat buffer;
    return (stat(path->data, &buffer) == 0);
#endif
}

bool path_file_new(const path_os_t* f_path ) {
    if (f_path == NULL) {
        fprintf(stderr, "[error] file_path is NULL\n");
        return false;
    }

    FILE* file = fopen(f_path->data, "w");
    if (file == NULL) {
        perror("Failed to create file");
        return false;
    }

    fclose(file);
    return true;
}

bool path_dir_new(const path_os_t* dir) {
    if (dir == NULL) {
        fprintf(stderr, "[error] dir_path is NULL\n");
        return false;
    }

#ifdef _WIN32
    char cmd[MAX_PATH];
    snprintf(cmd, sizeof(cmd), "mkdir %s", dir->data);
    return system(cmd) == 0;
#else
    char cmd[PATH_MAX];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", dir->data);
    return system(cmd) == 0;
#endif
}

const char* path_get_last_dir(const path_os_t* dir) {
    if (dir == NULL || dir->data[0] == '\0') {
        return NULL;
    }

    const char* last_sep_forward = strrchr(dir->data, '/');
    const char* last_sep_backward = strrchr(dir->data, '\\');
    const char* last_sep = last_sep_forward > last_sep_backward ? last_sep_forward : last_sep_backward;

    if (last_sep == NULL) {
        return dir->data; 
    }

    if (*(last_sep + 1) == '\0') {
        const char* end = last_sep;
        last_sep = end - 1;
        while (last_sep >= dir->data && *last_sep != '/' && *last_sep != '\\') {
            last_sep--;
        }
        if (last_sep < dir->data || last_sep == end - 1) {
            return NULL; 
        }
    }
     
    return last_sep + 1;
}

const char* path_get_ext(const path_os_t* path) {
    if (path == NULL || path->data[0] == '\0') {
        return NULL;
    }

    const char* filename = path_get_last_dir(path);
    if (!filename) {
        return NULL;
    }

    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return NULL;
    }

    return dot + 1;
}

bool path_set_sys_dir(const path_os_t* path) {
#ifdef _WIN32
    wchar_t buffer[260] = { 0 };
    cstr_to_wchar(path->data, buffer);
    if (SetCurrentDirectory(buffer)) {
        return true;
    }
    else {
        printf("Failed to change directory to %s\n", path->data);
    }
#else
    if (chdir(path.data) == 0) {
        return true;
    }
    else {
        perror("Failed to change directory");
    }
#endif
    return true;
}

void path_file_close() {
    free(file_cache);
    file_cache = NULL;
}

bool path_file_open(const path_os_t* path, char* file_buffer, size_t* file_length) {
    if (file_cache) path_file_close();

    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        printf("failed to open file %s!\n", path->data);
        return false;
    }


    size_t size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    file_cache = (char*)malloc(size);
    if (file_cache == NULL) {
        fclose(file);
        return NULL;
    }

    fread(file_cache, 1, size, file);
    fclose(file);

    if (file_length) {
        *file_length = size;
    }

    file_buffer = file_cache;
   
    return true;
}