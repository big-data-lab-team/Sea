#ifndef SEA_H_
#define SEA_H_

// taken from https://stackoverflow.com/questions/54520101/xstat-dynamic-symbol-resolving-error-on-64bit
#ifndef __x86_64__
# define _STAT_VER_LINUX    3
#else
# define _STAT_VER_LINUX    1
#endif

#include <limits.h>
#include <vector>
#include <dirent.h>

extern int sea_internal;
int set_internal();
int get_internal();
int unset_internal();
extern std::vector<char *> sea_files;
int sea_checkpath(const char* path); 
int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path);
int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path, int source_id);
void initialize_sea();
void initialize_sea_if_necessary();

struct SEA_DIR {
    DIR* dirp;
    DIR** other_dirp;
    char type[7] = "seadir"; // mostly useless. only used in closedir to ensure struct type is right
    char** dirnames;
    int curr_index = 0;
};

#endif

