#include "config.h"
#include "logger.h"
#include "sea.h"
#include "passthrough.h"
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <cstdlib>
#include <algorithm>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <utime.h>
#include <unistd.h>

int sea_internal;

// need to convert to C structs
const char *fdpath = "/proc/self/fd";

/**
 * Checks if path exists by performing an __xstat
 *
 * @param path the path to verify
 * @return 1 if path exists or 0 if it does not
 */
int sea_checkpath(const char *path)
{
    return (((funcptr_access)libc_access)(path, F_OK) == 0);
}

/**
 *
 * Overloaded function of sea_getpath. Returns the true path of the file if located within the mountpoint.
 * Can also return the "masked" (path relative to mountpoint).
 *
 * @param oldpath the original function path
 * @param passpath the true path of the file (if located in a sea mountpoint)
 * @param masked_path whether to populate passpath with the real source mount location or with the "masked" mountpoint path
 * @return whether oldpath was located in a sea mountpoint or not.
 *
 */
int sea_getpath(const char *oldpath, char passpath[PATH_MAX], int masked_path)
{

    return sea_getpath(oldpath, passpath, masked_path, -1);
}

/**
 *
 * "Main" overloaded function that calls passthrough_getpath to get real path.
 * Returns the true path of the file if located within the mountpoint.
 * Can also return the "masked" (path relative to mountpoint).
 *
 * @param oldpath the original function path
 * @param passpath the true path of the file (if located in a sea mountpoint)
 * @param masked_path whether to populate passpath with the real source mount location or with the "masked" mountpoint path
 * @param sea_lvl specifies the index of the source mount to use. If -1, will go through all possible source_mounts to look for existing path
 * @return whether oldpath was located in a sea mountpoint or not.
 *
 */
int sea_getpath(const char *oldpath, char passpath[PATH_MAX], int masked_path, int sea_lvl)
{
    struct config sea_config = get_sea_config();

    int match = 0;
    int exists = 0;

    if (sea_lvl != -1)
    {
        match = pass_getpath(oldpath, passpath, masked_path, sea_lvl);
        return match;
        //printf("match %d %s\n", match, passpath);
    }
    else
    {
        for (int i = 0; i < sea_config.n_sources; ++i)
        {
            match = pass_getpath(oldpath, passpath, masked_path, i);
            //printf("passpath %s\n", passpath);

            if (masked_path == 0 && match == 1)
            {
                exists = sea_checkpath(passpath);
                //printf("exists %d %s\n", exists, passpath);

                if (exists)
                {
                    return match;
                }
            }
            else if (masked_path == 1)
            {
                return match;
            }
        }
    }

    if (match == 1 && exists == 0)
    {
        for (int i = 0; i < sea_config.n_sources; ++i)
        {
            struct statvfs buf;
            int ret;
            if ((ret = ((funcptr_statvfs)libc_statvfs)(sea_config.source_mounts[i], &buf) == 0) && (buf.f_bavail * buf.f_bsize > sea_config.max_fs * sea_config.n_threads))
            {
                match = pass_getpath(oldpath, passpath, masked_path, i);
                return match;
            }
        }
    }

    if (match == 0)
    {
        //passpath = NULL;
        strcpy(passpath, oldpath);
    }

    return match;
}

/**
 * Directories which do not exist in all source mounts are created in all the mounts.
 *
 * @param basePath the root directory to start adding paths from
 * @param sea_lvl the index of the basePath's parent source mount
 * @param sea_config the sea configuration struct
 *
 */
void mirrorSourceDirs(char *basePath, int sea_lvl, struct config sea_config)
{
    log_msg(DEBUG, "mirrorSourceDirs: Attempting to mirror directories at %s", basePath);
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

        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            if (dp->d_type == DT_DIR)
            {
                struct stat buf;
                if (((funcptr___xstat)libc___xstat)(_STAT_VER_LINUX, path, &buf) == 0)
                {

                    for (int i = 0; i < sea_config.n_sources; ++i)
                    {
                        if (i != sea_lvl)
                        {
                            char dir_to_create[PATH_MAX];

                            strcpy(dir_to_create, sea_config.source_mounts[i]);

                            char *match;
                            int len = strlen(sea_config.source_mounts[sea_lvl]);
                            char *tmp = strdup(basePath);

                            if ((match = strstr(tmp, sea_config.source_mounts[sea_lvl])))
                            {
                                if (match != NULL && match[0] != '\0')
                                {
                                    *match = '\0';
                                    strcat(dir_to_create, match + len);
                                }
                            }

                            strcat(dir_to_create, "/");
                            strcat(dir_to_create, dp->d_name);

                            //TODO: add error handling here
                            log_msg(INFO, "mirrorSourceDirs: Creating dir %s %d", dir_to_create, buf.st_mode);
                            ((funcptr_mkdir)libc_mkdir)(dir_to_create, buf.st_mode);
                            // don't care if directory already exists
                            if (errno == EEXIST)
                                errno = 0;
                        }
                    }
                }
            }
            mirrorSourceDirs(path, sea_lvl, sea_config);
        }
    }
    log_msg(DEBUG, "mirrorSourceDirs: Completed");
    ((funcptr_closedir)libc_closedir)(dir);
}

/**
 * Populate the sea_files set with all the directories and paths located within the
 * source mounts
 *
 */
void initialize_sea()
{
    struct config sea_config = get_sea_config();

    for (int i = 0; i < sea_config.n_sources; i++)
    {
        mirrorSourceDirs(sea_config.source_mounts[i], i, sea_config);
        //printf("sources %s\n", sea_config.source_mounts[i]);
    }
}

static pthread_once_t sea_initialized = PTHREAD_ONCE_INIT;

/**
 * Create a pthread to intialize the sea_files set. Doesn't really work, so also
 * check if there are no files in the sea_files set and if so, call initialize_sea 
 *
 */
void initialize_sea_if_necessary()
{
    pthread_once(&sea_initialized, initialize_sea);
}
