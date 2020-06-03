#include <limits.h>
#include <vector>

extern std::vector<char*> sea_files;
int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path);
void initialize_sea();
void initialize_sea_if_necessary();
