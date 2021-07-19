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
