#ifndef SEA_H_
#define SEA_H_

// taken from https://stackoverflow.com/questions/54520101/xstat-dynamic-symbol-resolving-error-on-64bit
#ifndef __x86_64__
#define _STAT_VER_LINUX 3
#else
#define _STAT_VER_LINUX 1
#endif

#include <map>
#include <vector>
#include <limits.h>
#include <set>
#include <string>
#include <dirent.h>

int sea_checkpath(const char *path);
int sea_getpath(const char *oldpath, char passpath[PATH_MAX], int masked_path);
int sea_getpath(const char *oldpath, char passpath[PATH_MAX], int masked_path, int source_id);
extern const char *fdpath;

struct SEA_DIR
{
    DIR *dirp;
    DIR **other_dirp;
    int issea = 0; // mostly useless. only used in closedir to ensure struct type is right
    char **dirnames;
    int curr_index;
    int total_dp;
    std::set<std::string> seen_files; // bandaid solution to not listing duplicate files in mount
};

#endif
