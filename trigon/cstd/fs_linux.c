#ifdef _LINUX
#include "cstd.h"

#ifdef _WIN32 
#define __FS_STD_DIREND '\\'
#define __FS_NSTD_DIREND '/'
#else
#define __FS_STD_DIREND '/'
#define __FS_NSTD_DIREND '\\'
#endif

#include <unistd.h> 
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void fs_parent(fs_t out, fs_t path) {
    int len = strlen(path);
    strcpy(out, path);
    for (int i = len; i > 0; i--) {
        if (out[i] == __FS_STD_DIREND) {
            out[i + 1] = '\0';
            return;
        }
    }
}

// get application location
void fs_app(fs_t p) {
    readlink("/proc/self/exe", p, __TG_FS_MAX);
}

// get current working directory
void fs_dir(fs_t p) { getcwd(p, __TG_FS_MAX); }

//if is file
bool fs_isfile(fs_t in) {
    struct stat path_stat = { 0 };
    stat(in, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

// get user home
void fs_usr(fs_t out) {
    const char* home = getenv("HOME");
    if (home) strcpy(out, home);
    else {
        const char* user_profile = getenv("USERPROFILE");
        if (user_profile) {
            strcpy(out, user_profile);
        }
        else {
            out[0] = '\0';
        }
    }
}

// get system root path (C:\ or /)
void fs_root(fs_t out) {
#ifdef __unix__
    strcpy(out, "/");
#else
    strcpy(out, "C:\\");
#endif
}

// if file or dirtory exists
bool fs_exists(fs_t in) {
    if (fs_isfile(in)) return access(in, F_OK) == 0;
    else return opendir(in) != NULL;
}

// append custom value to path
void fs_append(fs_t in, fs_t ext) {
    strcat(in, ext);
}

// validates the path, using correct std endings
void fs_valid(fs_t in) {
    int str_len = strlen(in);
    for (int i = 0; i < str_len; i++)
        if (in[i] == __FS_NSTD_DIREND) in[i] = __FS_STD_DIREND;

    if (!fs_exists(in)) return;

    if (fs_isfile(in)) {
        if (in[str_len - 1] != __FS_STD_DIREND) return;
        in[str_len - 1] = '\0';
    }
    else {
        if (in[str_len - 1] == __FS_STD_DIREND) return;
        in[str_len] = __FS_STD_DIREND;
        in[str_len + 1] = '\0';
    }

}

bool fs_find(fs_t path, fs_t child) {
    fs_t tmp = { 0 };
    fs_cp(tmp, path);
    fs_valid(tmp);
    fs_append(tmp, child);
    return fs_exists(tmp);
}

// returns a path if it exists, creates if missing, add '/' | '\\' in the end
// to make a dir
void fs_get(fs_t path, fs_t child) {
    fs_valid(path);
    fs_append(path, child);

    if (fs_exists(path)) {
        fs_valid(path);
        return;
    }

    if (path[strlen(path) - 1] == __FS_STD_DIREND) {
        mkdir(path, 0700);
        return;
    }

    FILE* f = fopen(path, "w");
    if (!f) return;
    fclose(f);
}

void fs_ext(fs_t out, fs_t p) {
    const char* dot = strrchr(p, '.');
    if (!dot || dot == p) {
        memset(out, 0, sizeof(fs_t));
        return;
    }

    strcpy(out, dot + 1);
}

//returns name of file
void fs_name(fs_t out, fs_t p) {
    const char* filename = strrchr(p, '/');
    if (filename) {
        filename++;
    }
    else {
        filename = p;
    }

    const char* ext = strrchr(filename, '.');
    size_t length;
    if (ext) {
        length = ext - filename;
    }
    else {
        length = strlen(filename);
    }

    if (length >= __TG_FS_MAX) {
        length = __TG_FS_MAX - 1;
    }

    strncpy(out, filename, length);
    out[length] = '\0';
}

//returns relative path
void fs_rel(fs_t dest, fs_t ancestor, fs_t descandant) {
    memset(dest, 0, __TG_FS_MAX);
    int tl = 0;
    int al = strlen(ancestor);
    int ad = strlen(descandant);
    int mx = MAX(al, ad);
    for (int i = 0; i < mx; i++) {
        if (i < ad && i < al && ancestor[i] == descandant[i]) continue;
        dest[tl] = descandant[i];
        tl++;
    }
}

static int fs_lsrec(fs_t path, fs_t* out, const char* exc_ext, bool recursive, int* len) {
    DIR* d = opendir(path);
    if (!d) return 0;

    struct dirent* dir;
    fs_t e = { 0 };
    fs_t sub = { 0 };
    while ((dir = readdir(d)) != NULL) {
        if (strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0) {
            continue;
        }

        memset(sub, 0, sizeof(fs_t));
        strcpy(sub, path);
        fs_valid(sub);
        strcat(sub, dir->d_name);

        if (!fs_isfile(sub) && recursive) {
            fs_lsrec(sub, out, exc_ext, recursive, len);
        }
        else {
            if (exc_ext != NULL) {
                fs_ext(e, dir->d_name);
                if (strcmp(e, exc_ext) != 0) {
                    continue;
                }
            }
            if (out) {
                strcpy(out[*len], sub);
            }
            (*len)++;
        }
    }

    closedir(d);
    return *len;
}

//list all files in directory, out can be NULL to only get coun,
//exc_ext = blacklist extension
int fs_ls(fs_t path, fs_t* out, const char* exc_ext, bool recursive) {
    int l = 0;
    fs_lsrec(path, out, exc_ext, recursive, &l);
    return l;
}

void fs_cp(fs_t dest, fs_t src) {
    strcpy(dest, src);
}

#endif

