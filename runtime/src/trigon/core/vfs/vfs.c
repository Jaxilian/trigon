#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#endif

void vfs_to_unix(vpath_t path) {
    uint32_t len = (uint32_t)strlen(path);

    for (uint32_t i = 0; i < len; i++) {
        if (path[i] == WIN_PATH_END_STD) {
            path[i] = UNIX_PATH_END_STD;
        }
    }
}

void vfs_to_win32(vpath_t path) {
    uint32_t len = (uint32_t)strlen(path);

    for (uint32_t i = 0; i < len; i++) {
        if (path[i] == UNIX_PATH_END_STD) {
            path[i] = WIN_PATH_END_STD;
        }
    }
}

uint32_t vfs_split(vpath_t in, const char* delimiter, vpath_t* out) {
    char* str_cpy = strdup(in);

    if (str_cpy == NULL) {
        printf("failed to allocate memory for copy\n");
        return false;
    }

    char* token = strtok(str_cpy, delimiter);

    int buffer_index = 0;
    while (token != NULL) {

        if (out) {
            strcpy(out[buffer_index], token);
        }

        buffer_index++;
        token = strtok(NULL, (const char*)delimiter);
    }

    free(str_cpy);
    return buffer_index;
}

bool cstr_to_wchar(const char* str, wchar_t* wchar) {
#ifdef _WIN32
    size_t len = 0;
    errno_t err = mbstowcs_s(&len, NULL, 0, str, _TRUNCATE);
    if (err != 0 || len == 0) {
        return false;
    }

    size_t converted = 0;
    err = mbstowcs_s(&converted, wchar, len, str, _TRUNCATE);
    if (err != 0 || converted == 0) {
        return false;
    }
#else
    size_t len = mbstowcs(NULL, str, 0);
    if (len == (size_t)(-1)) return false;
    mbstowcs(wchar, str, len + 1);
#endif

    return true;
}

bool wchar_to_cstr(const wchar_t* wchar, char* str) {
#ifdef _WIN32
    size_t len = 0;
    errno_t err = wcstombs_s(&len, NULL, 0, wchar, _TRUNCATE);

    if (err != 0 || len == 0) {
        return false;
    }

    size_t converted = 0;
    err = wcstombs_s(&converted, str, len, wchar, _TRUNCATE);
    if (err != 0 || converted == 0) {
        return false;
    }
#else
    size_t len = wcstombs(NULL, wchar, 0);
    if (len == (size_t)(-1)) return false;
    wcstombs(str, wchar, len + 1);
#endif

    return true;
}

bool vfs_is(const vpath_t path, int type) {
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(path);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    switch (type) {
    case VFS_TYPE_FILE:
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    case VFS_TYPE_DIR:
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    default:
        return false;
    }
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }

    switch (type) {
    case VFS_TYPE_FILE:
        return S_ISREG(st.st_mode);
    case VFS_TYPE_DIR:
        return S_ISDIR(st.st_mode);
    default:
        return false;
    }
#endif 
}

static void validate_path(vpath_t path) {
    if (!vfs_is(path, VFS_TYPE_DIR)) return;

#ifdef _WIN32
    uint32_t len = (uint32_t)strlen(path);
    for (uint32_t i = 0; i < len; i++) {
        if (path[i] == UNIX_PATH_END_STD) {
            path[i] = WIN_PATH_END_STD;
        }
    }

    char lastchar = path[len - 1];
    if (lastchar != WIN_PATH_END_STD && lastchar != UNIX_PATH_END_STD) {
        path[len] = STD_PATH_END;
        path[len + 1] = '\0';
    }
#else
    uint32_t len = (uint32_t)strlen(path);
    for (uint32_t i = 0; i < len; i++) {
        if (path[i] == '\\') {
            path[i] = '/';
        }
    }
    uint32_t parent_length = (uint32_t)strlen(path);
    char lastchar = path[parent_length - 1];
    if (lastchar != '/') {
        strcat_s(path, sizeof(vpath_t), "/");
    }
#endif
}

bool vfs_exist(const vpath_t path) {
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributesA(path);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES);
#else
    return access(path, F_OK) == 0;
#endif
}

bool vfs_create(const vpath_t parent, const char* name, VFS_TYPE type) {
    vpath_t new_path = { 0 };
    strcpy_s(new_path, sizeof(vpath_t), parent);

    validate_path(new_path);
    strcat_s(new_path, sizeof(vpath_t), name);

    if (type == VFS_TYPE_DIR) {
        if (vfs_exist(new_path)) {
            return false;
        }

        char cmd[MAX_PATH + 9];
        snprintf(cmd, sizeof(cmd), "mkdir %s", new_path);
        return system(cmd) == 0;
    }
    else if (type == VFS_TYPE_FILE) {
        FILE* file;
        errno_t err = fopen_s(&file, new_path, "w");
        if (err != 0) {
            printf("failed to create file: %s\n", new_path);
            return false;
        }

        fclose(file);
    }

    return true;
}

bool vfs_new(const char* cstr_path, vpath_t out) {
    if (!vfs_exist(cstr_path)) {
        printf("path: %s doesn't exist! create it first\n", cstr_path);
        return false;
    }
    memset(out, 0, sizeof(vpath_t));
    strcpy_s(out, sizeof(vpath_t), cstr_path);

    validate_path(out);
    return true;
}

bool vfs_system_root(vpath_t out) {
    memset(out, 0, sizeof(vpath_t));

#ifdef _WIN32
    strcpy_s(out, sizeof(vpath_t), "C:\\");
#else
    strcpy_s(out, sizeof(vpath_t), "/");
#endif

    return true;
}

bool vfs_user_home(vpath_t out) {
    memset(out, 0, sizeof(vpath_t));

#ifdef _WIN32
    char win_app_data[MAX_PATH];
    if (GetEnvironmentVariableA("LOCALAPPDATA", win_app_data, MAX_PATH)) {
        strcpy_s(out, sizeof(vpath_t), win_app_data);
        return true;
    }
    else {
        printf("error getting \"Local AppData\" path\n");
        return false;
    }
#else
    char* home = getenv("HOME");
    if (home != NULL) {
        strcpy_s(out, sizeof(vpath_t), home);
        return true;
    }
    else {
        printf("error getting \"Home\" path\n");
        return false;
    }
#endif
}

bool vfs_depth_len(const vpath_t path, uint32_t* out_depth) {
    if (out_depth == NULL) {
        return false;
    }

    uint32_t found = 0;
    for (uint32_t i = 0; i < strlen(path); ++i) {
        if (path[i] == '\\' || path[i] == '/') {
            found++;
        }
    }

    *out_depth = found;
    return true;
}

bool vfs_cd(const vpath_t path) {
#ifdef _WIN32
    wchar_t buffer[260] = { 0 };
    cstr_to_wchar(path, buffer);
    if (SetCurrentDirectoryW(buffer)) {
        return true;
    }
    else {
        printf("failed to change directory to %s\n", path);
    }
#else
    if (chdir(path) == 0) {
        return true;
    }
    else {
        perror("failed to change directory");
    }
#endif
    return false;
}

uint32_t vfs_divide(const vpath_t path, vpath_t* out) {
    uint32_t len = 0;
    vfs_depth_len(path, &len);
    if (out == NULL) {
        return len;
    }

    uint32_t index = 0;
    uint32_t cache_len = 0;
    vpath_t cache = { 0 };

    for (uint32_t i = 0; path[i] != '\0' && index < len; ++i) {
        if (path[i] == '\\' || path[i] == '/') {
            if (cache_len > 0) {
                cache[cache_len] = '\0';
                strcpy_s(out[index++], sizeof(vpath_t), cache);
                cache_len = 0;
            }
        }
        else {
            if (cache_len < MAX_PATH - 1) {
                cache[cache_len++] = path[i];
            }
        }
    }

    if (cache_len > 0 && index < len) {
        cache[cache_len] = '\0';
        strcpy_s(out[index], sizeof(vpath_t), cache);
    }

    return len;
}

bool vfs_find_path(const vpath_t path, bool create_missing) {
    bool found = false;
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributesA(path);
    found = (dwAttrib != INVALID_FILE_ATTRIBUTES);
#else
    DIR* dir = opendir(path);
    if (dir) {
        found = true;
        closedir(dir);
    }
#endif

    if (create_missing && !found) {
        return vfs_create(path, "", VFS_TYPE_DIR);
    }

    return found;
}

bool vfs_current_path(vpath_t out) {
#ifdef _WIN32
    WCHAR buff[MAX_PATH] = { 0 };
    GetCurrentDirectoryW(MAX_PATH, buff);
    wchar_to_cstr(buff, out);
#else
    if (getcwd(out, MAX_PATH) == NULL) {
        perror("getcwd");
        return false;
    }
#endif
    return true;
}

bool vfs_extension(const char* filename, vpath_t outext) {
    const char* dot = strrchr(filename, '.');
    if (!dot) {
        return false;
    }
    strcpy(outext, dot + 1);
    return true;
}

bool vfs_filename(vpath_t path, vpath_t out, bool remove_extension) {
    if (!vfs_is(path, VFS_TYPE_FILE)) {
        return false;
    }

    const char* file_name = strrchr(path, STD_PATH_END);
    const char* result = file_name;

    if (file_name) {
        result = file_name + 1;
    }
    else {
        return false;
    }
    strcpy_s(out, sizeof(vpath_t), result);

    if (remove_extension) {

        vpath_t* others = NULL;
        uint32_t ssplit = vfs_split(result, ".", others);
        if (ssplit <= 1) return true;

        others = malloc(sizeof(vpath_t) * ssplit);
        if (!others) return false;

        ssplit = vfs_split(result, ".", others);

        strcpy_s(out, sizeof(vpath_t), others[0]);
        free(others);
    }

    return true;
}

static uint32_t find_paths(vpath_t parent, vpath_t* paths, bool find_file, bool ignore_hidden, const char* extension) {
    validate_path(parent);
    int count = 0;

    WCHAR search_path[MAX_PATH];
    _snwprintf_s(search_path, MAX_PATH, _TRUNCATE, L"%S*", parent);

    WIN32_FIND_DATA find_file_data;
    HANDLE h_find = FindFirstFile(search_path, &find_file_data);

    if (h_find == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "find child failed (%lu)\n", GetLastError());
        return 0;
    }

    do {
        if ((find_file && (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
            (!find_file && !(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) {
            continue;
        }

        if (find_file && extension) {
            char buff[MAX_PATH] = { 0 };
            wchar_to_cstr(find_file_data.cFileName, buff);

            vpath_t ext = { 0 };
            vfs_extension(buff, ext);

            if (strcmp(ext, extension) != 0) continue;
        }

        if (!find_file) {
            bool thisandback = strcmp(find_file_data.cFileName, ".") == 0 || strcmp(find_file_data.cFileName, "..") == 0;
            if (ignore_hidden && thisandback) continue;
        }



        WCHAR full_path_wide[MAX_PATH];
        _snwprintf_s(full_path_wide, MAX_PATH, _TRUNCATE, L"%S%s", parent, find_file_data.cFileName);


        if (paths) {
            size_t converted_chars = 0;
            errno_t conversion_result = wcstombs_s(&converted_chars, paths[count], MAX_PATH, full_path_wide, _TRUNCATE);
            validate_path(paths[count]);
            if (conversion_result != 0) {
                perror("Error in converting wide char to multibyte.\n");
                continue;
            }

        }


        count++;

    } while (FindNextFile(h_find, &find_file_data) != 0);

    FindClose(h_find);
    return count;
}

uint32_t vfs_files(vpath_t parent, vpath_t* files, const char* extension) {
    return find_paths(parent, files, true, true, extension);
}

uint32_t vfs_dirs(vpath_t parent, vpath_t* dirs) {
    return find_paths(parent, dirs, false, true, NULL);
}

static void fetch_all(vpath_t parent, uint32_t* count, vpath_t* out, const char* extension) {

    uint32_t child_path_count = vfs_dirs(parent, NULL);
    vpath_t* child_paths = calloc(child_path_count, sizeof(vpath_t));
    if (!child_paths) return;

    if (child_path_count > 0) {
        vfs_dirs(parent, child_paths);
    }

    for (uint32_t i = 0; i < child_path_count; i++) {
        fetch_all(child_paths[i], count, out, extension);
    }

    uint32_t child_file_count = vfs_files(parent, NULL, extension);

    vpath_t* files = NULL;
    if (out) {
        files = calloc(child_file_count, sizeof(vpath_t));
        vfs_files(parent, files, extension);
    }

    for (uint32_t i = 0; i < child_file_count; i++) {
        if (out && files) {
            strcpy_s(out[*count], sizeof(vpath_t), files[i]);
        }
        (*count)++;
    }

    if (files) free(files);
    if (child_paths) free(child_paths);

}

uint32_t vfs_all_files(const vpath_t parent, vpath_t* files, const char* extension) {
    uint32_t total_files = 0;
    fetch_all(parent, &total_files, files, extension);
    return total_files;
}

bool vfs_extend_path(const vpath_t parent, const char* child, vpath_t out) {
    strcpy_s(out, sizeof(vpath_t), parent);
    strcat_s(out, sizeof(vpath_t), "/");
    strcat_s(out, sizeof(vpath_t), child);
    return true;
}


bool vfs_find(vpath_t path, vpath_t outpath, const char* name, bool recursive) {

    uint32_t    file_count      = 0;
    vpath_t*    files           = NULL;
    bool        has_extension   = false;
    vpath_t     extension       = { 0 };
    vpath_t     filename        = { 0 };

    if (recursive) {
        file_count = vfs_all_files(path, NULL, NULL);
        if (file_count <= 0) return false;

        files = calloc(file_count, sizeof(vpath_t));
        if (!files) return false;

        vfs_all_files(path, files, NULL);
        has_extension = vfs_extension(name, extension);

        for (uint32_t i = 0; i < file_count; i++) {
           
            vfs_filename(files[i], filename, !has_extension);

            if (strcmp(filename, name) == 0) {
                strcpy_s(outpath, sizeof(vpath_t), files[i]);
                free(files);
                return true;
            }
        }
    }
    else {
        file_count = vfs_files(path, NULL, NULL);
        if (file_count <= 0) return false;

        files = calloc(file_count, sizeof(vpath_t));
        if (!files) return false;

        vfs_files(path, files, NULL);
        has_extension = vfs_extension(name, extension);

        for (uint32_t i = 0; i < file_count; i++) {

            vfs_filename(files[i], filename, !has_extension);

            if (strcmp(filename, name) == 0) {
                strcpy_s(outpath, sizeof(vpath_t), files[i]);
                free(files);
                return true;
            }
        }
    }


    free(files);
    return false;
}

