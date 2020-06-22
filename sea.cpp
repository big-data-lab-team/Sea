#include "config.h"
#include "logger.h"
#include "sea.h"
#include "passthrough.h"
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <cstdlib>

int sea_internal;

std::vector<char *> sea_files;

int get_internal() {
    return sea_internal;
}
int set_internal() {
    sea_internal = 1;
    return sea_internal;
}

int unset_internal() {
    sea_internal = 0;
    return sea_internal;
}

int sea_checkpath(const char* path) {
    struct stat buf;

    errno = 0;
    //printf("checkpath %s\n", path);
    set_internal();
    int ret = 0;
    if((ret = __xstat(_STAT_VER_LINUX, path, &buf)) != 0) {
        //printf("errno %d\n", errno);
        return 0;
    }
    unset_internal();
    return 1;
}

int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path) {

    return sea_getpath(oldpath, passpath, masked_path, -1);
}

int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path, int sea_lvl) {
    struct config sea_config = get_sea_config();

    int match = 0;
    char tmp_passpath[PATH_MAX] = { '\0' };

    if (sea_lvl != -1) {
        match = pass_getpath(oldpath, passpath, masked_path, sea_lvl);
        //printf("match %d %s\n", match, passpath);
    }
    else {
        for ( int i=0 ; i < sea_config.n_sources; ++i ) {
            match = pass_getpath(oldpath, passpath, masked_path, i);
            //printf("passpath %s\n", passpath);

            if ( masked_path == 0 && match == 1) {
                int exists = sea_checkpath(passpath);
                //printf("exists %d\n", exists);

                if (exists)
                    return match; 
                else if (tmp_passpath[0] == '\0') {
                    // if doesn't exist, create at top of hierarchy
                    strcpy(tmp_passpath, passpath);
                }
            }
            else if ( masked_path == 1 ) {
                return match;
            }
        }
    }

    if (match == 1 && tmp_passpath[0] != '\0')
        strcpy(passpath, tmp_passpath);
    
    if (match == 0) {
        strcpy(passpath, oldpath);
    }

    //printf("returned passpath %s\n", passpath);
    
    return match;

}

//int sea_getpath(const char* oldpath, char passpath[PATH_MAX], int masked_path, int sea_lvl){
//
//    if(oldpath == NULL)
//        return 0;
//    
//    char actualpath[PATH_MAX];
//
//    // Get config
//    struct config sea_config = get_sea_config();
//    char * mount_dir = sea_config.mount_dir;
//    char ** source_mounts = sea_config.source_mounts;
//
//    char path[PATH_MAX];
//    get_pass_canonical(path, passpath, mount_dir, source_mounts[sea_lvl], masked_path);
//
//    int match_found = 0;
//
//
//    if (masked_path == 1){
//        for (int i=0; i < sea_config.n_sources ; i++){
//            char* match;
//            strcpy(path, source_mounts[i]);
//            strcpy(passpath, mount_dir);
//
//            int len = strlen(path);
//            if(path[0] != '\0' && (match = strstr(actualpath, path))){
//                if (match == NULL)
//                    log_msg(DEBUG, "match null");
//                log_msg(DEBUG, "match");
//                *match = '\0';
//                strcat(passpath, match + len);
//                match_found = 1;
//            }
//        }
//        if (match_found == 0) {
//            log_msg(DEBUG, "no match");
//            strcpy(passpath, oldpath);
//        }
//    }
//    else { 
//        strcpy(path, mount_dir);
//
//        //printf("x %s\n", curr_file);
//
//        char fpath_dup[PATH_MAX];
//        strcpy(fpath_dup, actualpath);
//        char* match;
//        int len = strlen(path);
//
//        //printf("ap %s, %s ,  %s\n", oldpath ,actualpath, path);
//        if(path[0] != '\0'  && (match = strstr(fpath_dup, path))){
//            if (match == NULL)
//                log_msg(DEBUG, "match null");
//            else {
//                log_msg(DEBUG, "match");
//                *match = '\0';
//
//                if (strcmp(match + len, "") == 0){
//                    match_found = 1;
//                    strcpy(passpath, sea_config.source_mounts[sea_lvl]);
//                    //printf("passpath %s\n", passpath);
//                }
//                else{
//
//                    for (char* curr_file: sea_files){
//                        //printf("values %s %s\n", curr_file, match + len);
//                        char match_str[PATH_MAX];
//                        int curr_len = strlen(curr_file);
//                        strcpy(match_str, match + len);
//                        int match_len = strlen(match_str);
//                        
//                        //printf("curr_path %s, match_str %s, len %d\n", curr_file, match_str, len);
//                        if ( strcmp(curr_file + curr_len - match_len, match_str) == 0) {
//                            match_found = 1;
//                            strcpy(passpath, curr_file);
//                            break;
//                        }
//                    }
//
//                    if (match_found == 0){
//                        log_msg(DEBUG, "new file");
//                        match_found = 1;
//                        strcpy(passpath, sea_config.source_mounts[sea_lvl]);
//                        strcat(passpath, match + len);
//
//                    }
//               }
//            }
//        }
//
//        if (match_found == 0) {
//            log_msg(DEBUG, "no match");
//            strcpy(passpath, oldpath);
//        }
//    }
//
//    log_msg(INFO, "old fn %s ---> new fn %s", oldpath, passpath);
//    return match_found;
//}

// obtained from : https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
// modified to populate vector
void populateFileVec(char *basePath, int sea_lvl, struct config sea_config)
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

        // Construct new path from our base path
        strcpy(path, basePath);
        strcat(path, "/");
        strcat(path, dp->d_name);
        printf("adding file %s\n", path);
        //char* fp = new char[PATH_MAX];
        //memcpy(fp, path, PATH_MAX);
        sea_files.push_back(strdup(path));
        //free(fp);

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            
            if (dp->d_type == DT_DIR) {
                char dir_to_create[PATH_MAX];

                struct stat buf;
                if( ((funcptr___xstat)libc___xstat)(_STAT_VER_LINUX, path, &buf) == 0) {
                    //printf("adding file %s\n", dp->d_name);

                    for (int i=0; i < sea_config.n_sources; ++i) {
                        if (i != sea_lvl) {
                            strcpy(dir_to_create, sea_config.source_mounts[i]);
                            strcat(dir_to_create, "/");
                            strcat(dir_to_create, dp->d_name);

                            //TODO: add error handling here
                            ((funcptr_mkdir)libc_mkdir)(dir_to_create, buf.st_mode);
                        }
                    } 
                }
            }
            populateFileVec(path, sea_lvl, sea_config);
        }
    }
    closedir(dir);
}

void initialize_sea(){

    sea_internal = 0;
    struct config sea_config = get_sea_config();
    char ** source_mounts = sea_config.source_mounts;

    for (int i=0; i < sea_config.n_sources; i++){
        //printf("source_mounts %s\n", source_mounts[i]);
        populateFileVec(source_mounts[i], i, sea_config);
    }

}

static pthread_once_t sea_initialized = PTHREAD_ONCE_INIT;

void initialize_sea_if_necessary() {
  initialize_sea();
  pthread_once(&sea_initialized, initialize_sea);
}
