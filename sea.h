#ifndef SEA_H_
#define SEA_H_

#include <limits.h>
#include <vector>
#include <dirent.h>

extern std::vector<char *> sea_files;
int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path);
int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path, int source_id);
void initialize_sea();
void initialize_sea_if_necessary();

struct SEA_DIR {
    DIR* dirp;
    DIR** other_dirp;
    char type[7] = "seadir"; // mostly useless. only used in closedir to ensure struct type is right
};

#endif

