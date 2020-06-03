#include "config.h"
#include "logger.h"
#include "sea.h"
#include "passthrough.h"
#include <pthread.h>
#include <string.h>


std::vector<char*> sea_files;

int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path){

    if(oldpath == NULL)
        return 0;
    
    char* match;
    char actualpath[PATH_MAX];

    // Get config
    struct config sea_config = get_sea_config();
    char * mount_dir = sea_config.mount_dir;
    char ** source_mounts = sea_config.source_mounts;

    if(oldpath[0]=='/')
    {
       strcpy(actualpath, oldpath);
    }
    else
    {
      make_file_name_canonical(oldpath, actualpath);
    }

    int match_found = 0;

    char path[PATH_MAX];

    if (masked_path == 1){
        for (int i=0; i < sea_config.n_sources ; i++){
            strcpy(path, source_mounts[i]);
            strcpy(passpath, mount_dir);

            int len = strlen(path);
            if(path[0] != '\0' && (match = strstr(actualpath, path))){
                if (match == NULL)
                    log_msg(DEBUG, "match null");
                log_msg(DEBUG, "match");
                *match = '\0';
                strcat(passpath, match + len);
                match_found = 1;
            }
        }
        if (match_found == 0) {
            log_msg(DEBUG, "no match");
            strcpy(passpath, oldpath);
        }
    }
    else { 
        strcpy(path, mount_dir);

        for (int i=0; i < sea_config.n_sources; i++){
            fprintf(stderr, "sea %s %d\n", source_mounts[i], i);

            strcpy(passpath, source_mounts[i]);
            int len = strlen(path);

            if(path[0] != '\0' && (match = strstr(actualpath, path))){
                if (match == NULL)
                    log_msg(DEBUG, "match null");
                log_msg(DEBUG, "match");
                *match = '\0';
                strcat(passpath, match + len);

                struct stat sb;
                
                fprintf(stderr, "sea %s %d\n", source_mounts[i], i);
                if (((funcptr_stat)libc_stat)(passpath, &sb) >= 0) {
                    match_found = 1;
                    break;
                }
            }
        }

        if (match_found == 0) {
            log_msg(DEBUG, "no match");
            strcpy(passpath, oldpath);
        }
    }

    log_msg(INFO, "old fn %s ---> new fn %s", oldpath, passpath);
    return match_found;
}

// obtained from : https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
// modified to populate vector
void populateFileVec(char *basePath)
{
    char path[PATH_MAX];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("adding file %s\n", dp->d_name);

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            sea_files.push_back(path);

            populateFileVec(path);
        }
    }

    closedir(dir);
}

void initialize_sea(){

    struct config sea_config = get_sea_config();
    char ** source_mounts = sea_config.source_mounts;

    for (int i=0; i < sea_config.n_sources; i++){
        populateFileVec(source_mounts[i]);
    }

    initialize_functions();
}

static pthread_once_t sea_initialized = PTHREAD_ONCE_INIT;

void initialize_sea_if_necessary() {
  pthread_once(&sea_initialized, initialize_sea);
}
