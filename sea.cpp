#include "config.h"
#include "logger.h"
#include "sea.h"
#include "passthrough.h"
#include <pthread.h>
#include <string.h>


std::vector<char*> sea_files;

int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path){

    return sea_getpath(oldpath, passpath, masked_path, 0);
}

int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path, int source_id){

    if(oldpath == NULL)
        return 0;
    
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
            char* match;
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

        //printf("x %s\n", curr_file);

        char fpath_dup[PATH_MAX];
        strcpy(fpath_dup, actualpath);
        char* match;
        int len = strlen(path);

        //printf("ap %s, %s ,  %s\n", oldpath ,actualpath, path);
        if(path[0] != '\0'  && (match = strstr(fpath_dup, path))){
            if (match == NULL)
                log_msg(DEBUG, "match null");
            else {
                log_msg(DEBUG, "match");
                *match = '\0';

                if (strcmp(match + len, "") == 0){
                    match_found = 1;
                    strcpy(passpath, sea_config.source_mounts[source_id]);
                    //printf("passpath %s\n", passpath);
                }
                else{

                    for (char* curr_file: sea_files){
                        //printf("values %s %s\n", curr_file, match + len);
                        char match_str[PATH_MAX];
                        int curr_len = strlen(curr_file);
                        strcpy(match_str, match + len);
                        int match_len = strlen(match_str);
                        
                        //printf("curr_path %s, match_str %s, len %d\n", curr_file, match_str, len);
                        if ( strcmp(curr_file + curr_len - match_len, match_str) == 0) {
                            match_found = 1;
                            strcpy(passpath, curr_file);
                            break;
                        }
                    }

                    if (match_found == 0){
                        log_msg(DEBUG, "new file");
                        match_found = 1;
                        strcpy(passpath, sea_config.source_mounts[source_id]);
                        strcat(passpath, match + len);

                    }
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
    //printf("base %s \n", basePath);
    char path[PATH_MAX];
    struct dirent *dp;
    DIR *dir = ((funcptr_opendir)libc_opendir)(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = ((funcptr_readdir)libc_readdir)(dir)) != NULL)
    {
        //printf("adding file %s\n", dp->d_name);

        // Construct new path from our base path
        strcpy(path, basePath);
        strcat(path, "/");
        strcat(path, dp->d_name);
        char* fp = new char[PATH_MAX];
        memcpy(fp, path, PATH_MAX);
        sea_files.push_back(fp);

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            populateFileVec(path);
        }
    }

    closedir(dir);
}

void initialize_sea(){

    struct config sea_config = get_sea_config();
    char ** source_mounts = sea_config.source_mounts;

    for (int i=0; i < sea_config.n_sources; i++){
        //printf("source_mounts %s\n", source_mounts[i]);
        populateFileVec(source_mounts[i]);
    }

}

static pthread_once_t sea_initialized = PTHREAD_ONCE_INIT;

void initialize_sea_if_necessary() {
  initialize_sea();
  pthread_once(&sea_initialized, initialize_sea);
}
