#include <string.h>
#include "functions.h"
#include "logger.h"
#include "config.h"
#include <stdlib.h>

/**
 * Function to setup passthrough/Sea environment and return corresponding passpath
 *
 * @param function the function name (only used for logging)
 * @param path the original path name
 * @param passpath the actual path for paths located in the mount directory
 * @param masked if the we should return a path from the view of the mount directory
 * @param nomatch return the original path if no match is found (necessary for functions take a directory fd as input)
 *
 */
void init_path(const char* function, const char* path, char passpath[PATH_MAX], int masked, int nomatch) {
    initialize_passthrough_if_necessary();
    int match = 0;
    
    if (!get_internal()) {
        log_msg(INFO, "Entering %s with path: %s", function, path);

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(path, passpath, masked);
        }
        else
            match = pass_getpath(path, passpath, masked);

        if (match == 0 && nomatch) {
            strcpy(passpath, path);
        }

        log_msg(INFO, "Completing %s with updated path  %s", function, passpath);
    }
    else {
        strcpy(passpath, path);
    }

}

/**
 * Function to setup passthrough/Sea environment and return corresponding passpath.
 * Will return absolute path in passpath variable as long as path is not NULL or . or ..
 *
 * @param function the function name (only used for logging)
 * @param path the original path name
 * @param passpath the actual path for paths located in the mount directory
 * @param masked if the we should return a path from the view of the mount directory
 *
 */
void init_path(const char* function, const char* path, char passpath[PATH_MAX], int masked) {
    init_path(function, path, passpath, masked, 0);
}

//TODO: maybe merge init_path with this one?
/**
 * Similar to init_path. Function to setup passthrough/Sea environment and return corresponding passpaths.
 * Necessary for functions that operate on two paths (e.g. rename and link)
 *
 * @param function the function name (only used for logging)
 * @param path the original path name
 * @param passpath the actual path for paths located in the mount directory
 * @param masked if the we should return a path from the view of the mount directory
 *
 */
void init_twopaths(const char* function, const char* oldpath, const char* newpath, char oldpasspath[PATH_MAX], char newpasspath[PATH_MAX], int nomatch){
    initialize_passthrough_if_necessary();

    if (!get_internal()) {
        log_msg(INFO, "Entering %s with paths: %s %s", function, oldpath, newpath);
        struct config sea_conf = get_sea_config();
        int match_old = 0;
        int match_new = 0;
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match_old = sea_getpath(oldpath, oldpasspath, 0);
            match_new = sea_getpath(newpath, newpasspath, 0);
        }
        else {
            match_old = pass_getpath(oldpath, oldpasspath, 0);
            match_new = pass_getpath(newpath, newpasspath, 0);
        }

        if (match_old == 0 && nomatch)
            strcpy(oldpasspath, oldpath);
        if (match_new == 0 && nomatch)
            strcpy(newpasspath, newpath);

        log_msg(INFO, "Completing %s with updated paths %s %s", function, oldpasspath, newpasspath);
    }
    else {
        strcpy(oldpasspath, oldpath);
        strcpy(newpasspath, newpath);
    }
}

extern "C" {

    int open(__const char* pathname, int flags, ...){
        char passpath[PATH_MAX];

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        init_path("open", pathname, passpath, 0);

        return ((funcptr_open)libc_open)(passpath, flags, mode);
    }

    int __open(__const char* pathname, int flags, ...){
        char passpath[PATH_MAX];

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        init_path("__open", pathname, passpath, 0);

        return ((funcptr___open)libc___open)(passpath, flags, mode);
    }

    int __open_2 (const char *file, int oflag){
        char passpath[PATH_MAX];
        
        init_path("__open_2", file, passpath, 0);

        return ((funcptr___open_2)libc___open_2)(passpath, oflag);

    }

    int open64(const char* pathname, int flags, ...){
        char passpath[PATH_MAX];

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        init_path("open64", pathname, passpath, 0);

        return ((funcptr_open64)libc_open64)(passpath, flags, mode);
    }

    int __open64(__const char* pathname, int flags, ...){
        char passpath[PATH_MAX];

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        init_path("__open64", pathname, passpath, 0);

        return ((funcptr___open64)libc___open64)(passpath, flags, mode);
    }

    int openat(int dirfd, const char* pathname, int flags, ...){
        char passpath[PATH_MAX];
    
        init_path("openat", pathname, passpath, 0, 1);

        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int openat64(int dirfd, const char* pathname, int flags, ...){
        char passpath[PATH_MAX];
        init_path("openat64", pathname, passpath, 0);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int __openat64_2(int dirfd, const char* pathname, int flags){
        char passpath[PATH_MAX];
        init_path("__openat64_2", pathname, passpath, 0);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    extern int __openat_2(int dirfd, const char* pathname, int flags){
        char passpath[PATH_MAX];
        init_path("__openat_2", pathname, passpath, 0);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int openat2(int dirfd, const char *pathname,
                   struct open_how *how, size_t size){
        char passpath[PATH_MAX];
        init_path("openat2", pathname, passpath, 0);
        return ((funcptr_openat2)libc_openat2)(dirfd, passpath, how, size);
    }

    //TODO: perhaps convert to use init_path. Leaving it for now
    DIR* opendir(const char* pathname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf;
        sea_conf = get_sea_config();

        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0, 0);

            // if not a directory within the mountpoint, don't need to create a SEA_DIR struct.
            if ( strcmp(passpath, pathname) == 0 )
                return ((funcptr_opendir)libc_opendir)(passpath);

            SEA_DIR *sd = new SEA_DIR;
            strcpy(sd->type, "seadir");
            sd->curr_index = 0;
            sd->dirnames = (char**) malloc(sizeof(char*) * sea_conf.n_sources * PATH_MAX);
            sd->other_dirp = (DIR**) malloc(sizeof(DIR*) * sea_conf.n_sources - 1);

            sd->dirp = ((funcptr_opendir)libc_opendir)(passpath); 

            sd->dirnames[0] = passpath;

            for (int i=1; i < sea_conf.n_sources; ++i) {
                sea_getpath(pathname, passpath, 0, i);
                sd->other_dirp[i-1] = ((funcptr_opendir)libc_opendir)(passpath); 
                sd->dirnames[i] = passpath;
                log_msg(INFO, "opening directory %s", passpath);
            }
            return (DIR*)sd;
        }
        else {
            pass_getpath(pathname, passpath, 0);
            return ((funcptr_opendir)libc_opendir)(passpath);
        }

    }

    //DIR* __opendir(const char* pathname){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(pathname, passpath);
    //    log_msg(INFO, "__opening directory %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    //DIR* opendir2(const char *name, int flags){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(name, passpath);
    //    log_msg(INFO, "opening directory 2 test %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    //DIR* __opendir2(const char *name, int flags){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(name, passpath);
    //    log_msg(INFO, "__opening directory 2 test %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    //DIR* opendirat (int dfd, const char *name){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(name, passpath);
    //    log_msg(INFO, "__opendirat %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}
    //DIR * __opendirat (int dfd, const char *name){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(name, passpath);
    //    log_msg(INFO, "__opendirat %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    //static DIR* __opendir_common(int fd, const char *name, int flags)
    //{
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(name, passpath);
    //    log_msg(INFO, "opening directory 2 test %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    int scandir(const char *dirp, struct dirent ***namelist,
                  int (*filter)(const struct dirent *),
                  int (*compar)(const struct dirent **, const struct dirent **)){
        char passpath[PATH_MAX];
        init_path("scandir", dirp, passpath, 0);
        return ((funcptr_scandir)libc_scandir)(passpath, namelist, filter, compar);
    }

    int scandir64(const char *dirp, struct dirent64 ***namelist,
                  int (*select)(const struct dirent64 *),
                  int (*cmp)(const struct dirent64 **, const struct dirent64 **)){
        char passpath[PATH_MAX];
        init_path("scandir64", dirp, passpath, 0);
        return ((funcptr_scandir64)libc_scandir64)(passpath, namelist, select, cmp);
    }

    //TODO: remove? definitely fix
    int scandirat (int dfd, const char *dir, struct dirent ***namelist,
             int (*select) (const struct dirent *),
             int (*cmp) (const struct dirent **, const struct dirent **))
    {
        initialize_passthrough_if_necessary();
        log_msg(INFO, "scandirat test");
        return 0;
    }

    //TODO: remove? definitely fix
    int scandirat64 (int dfd, const char *dir, struct dirent64 ***namelist,
             int (*select) (const struct dirent64 *),
             int (*cmp) (const struct dirent64 **, const struct dirent64 **))
    {
        initialize_passthrough_if_necessary();
        log_msg(INFO, "scandirat64 test");
        return 0;
    }

    struct dirent *sea_readnext(struct dirent* d, config sea_conf, SEA_DIR* sd) { 
        int found = 0;
        if (d != NULL && d->d_name[0] != '\0') {
            for (int i = 0 ; i < sea_conf.n_sources; ++i) {
                if (i != sd->curr_index + 1) {
                    char tmppath[PATH_MAX];
                    strcpy(tmppath, sea_conf.source_mounts[i]);

                    //if (sea_conf.source_mounts[i][strlen(sea_conf.source_mounts[i]) - 1] != '/')
                    strcat(tmppath, "/");

                    strcat(tmppath, d->d_name);

                    // no way around using the set here 
                    if (sea_files.find((std::string)tmppath) != sea_files.end()) {
                            found = 1;
                            break;
                    }
                }
            }
            if (found) {
                d = ((funcptr_readdir)libc_readdir)(sd->other_dirp[sd->curr_index]);
                d = sea_readnext(d, sea_conf, sd);
            
            }
        }
        //printf("found %d\n", found);
        return d;
    }

    //TODO: works (for now) but perhaps needs some cleaning up
    struct dirent *readdir(DIR *dirp){

        config sea_conf = get_sea_config();
        initialize_passthrough_if_necessary();
        struct dirent *d;
        log_msg(INFO, "readdir");
        errno = 0;
        
        if (sea_conf.parsed && sea_conf.n_sources > 1){

            SEA_DIR* sd = (SEA_DIR*) dirp;
            if (strcmp(sd->type, "seadir") == 0) {
                d = ((funcptr_readdir)libc_readdir)(sd->dirp);

                if (d == NULL && sea_conf.n_sources > 1 ) {

                    // skip display of "." and ".."
                    //((funcptr_readdir)libc_readdir)(sd->other_dirp[0]);

                    //printf("current index %d %d\n", sd->curr_index, sd->other_dirp[sd->curr_index]);
                    d = ((funcptr_readdir)libc_readdir)(sd->other_dirp[sd->curr_index]);

                    if (d == NULL && sd->curr_index + 1 < sea_conf.n_sources) {
                        sd->curr_index += 1;
                    }
                    //printf("curr_index %d\n", sd->curr_index);
                    d = sea_readnext(d, sea_conf, sd);


                    //printf("readdir %s %d\n", d->d_name, i);
                    
                    //read next files
                    //if (d != NULL && d->d_name[0] != '\0') {
                    //   printf("file %s\n", d->d_name); 
                    //}
                    //if (d != NULL) {
                    //    d = ((funcptr_readdir)libc_readdir)(sd->other_dirp[0]);

                    //    //printf("readdir %s\n", d->d_name);
                    //}
                }
           }
           else {
                d = ((funcptr_readdir)libc_readdir)(dirp);
           }
        }
        else {
            d = ((funcptr_readdir)libc_readdir)(dirp);
        }

        if (d == NULL && errno)
            log_msg(ERROR, "reading dir %s %s", d->d_name);
        log_msg(INFO, "end readdir");
        return d;
    }


#undef creat
    int creat(__const char *name, mode_t mode) {
      initialize_passthrough_if_necessary();
      log_msg(INFO, "creat %s", name);
      return open(name, O_CREAT | O_WRONLY | O_TRUNC, mode);
      //char passpath[PATH_MAX];
      //pass_getpath(name, passpath);
      //log_msg(INFO, "creat %s", name);
      //return ((funcptr_creat)libc_creat)(name, mode);
    }

    int creat64(__const char *name, mode_t mode) {
      initialize_passthrough_if_necessary();
      log_msg(INFO, "creat64 %s", name);
      return open(name, O_CREAT | O_WRONLY | O_TRUNC, mode);
      //char passpath[PATH_MAX];
      //pass_getpath(name, passpath);
      //log_msg(INFO, "creat64 %s", name);
      //return ((funcptr_creat64)libc_creat64)(name, mode);
    }

    int close(int fd){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "closing file");
        return ((funcptr_close)libc_close)(fd);
    }

    int closedir(DIR* dirp) {
        initialize_passthrough_if_necessary();
        config sea_conf = get_sea_config();

        if (sea_conf.parsed && sea_conf.n_sources > 1) {
            SEA_DIR* sd = (SEA_DIR*) dirp;
            //printf("closedir %d %d\n", sea_conf.parsed, sd->dirp == NULL);


            if (strcmp(sd->type, "seadir") == 0) {
                for (int i=1 ; i < sea_conf.n_sources; ++i) {
                    ((funcptr_closedir)libc_closedir)(sd->other_dirp[i-1]);
                }

                int ret = 0;
                ret = ((funcptr_closedir)libc_closedir)(sd->dirp);
                delete sd;
                return ret;

            }
            else {
                return ((funcptr_closedir)libc_closedir)(dirp);
            }

        }
        log_msg(INFO, "closedir");
        return ((funcptr_closedir)libc_closedir)(dirp);
    }

    ssize_t read(int fd, void *buf, size_t count){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "reading file of count %lu from fd %d", count, fd);

        size_t bytes = ((funcptr_read)libc_read)(fd, buf, count);
        return bytes;
    }

    ssize_t write(int fd, const void *buf, size_t count){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "writing file of count %lu from fd %d", count, fd);
        return ((funcptr_write)libc_write)(fd, buf, count);
    }

    ssize_t pread(int fd, void *buf, size_t count, off_t offset){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "preading file of count %lu", count);
        return ((funcptr_pread)libc_pread)(fd, buf, count, offset);
    }

    ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "pwriting file of count %lu", count);
        return ((funcptr_pwrite)libc_pwrite)(fd, buf, count, offset);
    }

    int dup(int oldfd){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "duplicating fd");
        return ((funcptr_dup)libc_dup)(oldfd);
    }

    int dup2(int oldfd, int newfd){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "duplicating 2 fd");
        return ((funcptr_dup2)libc_dup2)(oldfd, newfd);
    }

    off_t lseek(int fd, off_t offset, int whence){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "lseek");
        return ((funcptr_lseek)libc_lseek)(fd, offset, whence);
    }

    int mkdir(const char *pathname, mode_t mode){
        char passpath[PATH_MAX];
        init_path("mkdir", pathname, passpath, 0);
        return ((funcptr_mkdir)libc_mkdir)(passpath, mode);
    }

    int chdir(const char *pathname){
        char passpath[PATH_MAX];
        init_path("chdir", pathname, passpath, 0);
        return ((funcptr_chdir)libc_chdir)(passpath);
    }

    int dirfd(DIR* dirp) {
        initialize_passthrough_if_necessary();
        config sea_conf = get_sea_config();

        if (sea_conf.parsed && sea_conf.n_sources > 1) {
            SEA_DIR* sd = (SEA_DIR*) dirp;

            if (strcmp(sd->type, "seadir") == 0)
                return ((funcptr_dirfd)libc_dirfd)(sd->dirp);
        }
       return ((funcptr_dirfd)libc_dirfd)(dirp);
    }

    int rename(const char *oldpath, const char *newpath){
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        init_twopaths("rename", oldpath, newpath, oldpasspath, newpasspath, 0);
        return ((funcptr_rename)libc_rename)(oldpasspath, newpasspath);
    }

    int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        init_twopaths("renameat", oldpath, newpath, oldpasspath, newpasspath, 0);
        return ((funcptr_renameat)libc_renameat)(olddirfd, oldpasspath, newdirfd, newpasspath);
    }

    int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags){
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        init_twopaths("renameat2", oldpath, newpath, oldpasspath, newpasspath, 0);
        return ((funcptr_renameat2)libc_renameat2)(olddirfd, oldpasspath, newdirfd, newpasspath, flags);
    }

    int remove(const char *pathname){
        char passpath[PATH_MAX];
        init_path("remove", pathname, passpath, 0);
        return ((funcptr_remove)libc_remove)(passpath);
    }

    int link (const char *oldname, const char *newname){
        char old_passpath[PATH_MAX];
        char new_passpath[PATH_MAX];
        init_twopaths("link", oldname, newname, old_passpath, new_passpath, 0);
        return ((funcptr_link)libc_link)(old_passpath, new_passpath);
    }

    int linkat (int oldfd, const char *oldname, int newfd, const char *newname, int flags){
        char old_passpath[PATH_MAX];
        char new_passpath[PATH_MAX];
        init_twopaths("linkat", oldname, newname, old_passpath, new_passpath, 0);
        return ((funcptr_linkat)libc_linkat)(oldfd, old_passpath, newfd, new_passpath, flags);

    }

    int unlink(const char *pathname){
        char passpath[PATH_MAX];
        init_path("unlink", pathname, passpath, 0);
        return ((funcptr_unlink)libc_unlink)(passpath);
    }

    int unlinkat(int dirfd, const char *pathname, int flags){
        char passpath[PATH_MAX];
        init_path("unlinkat", pathname, passpath, 0, 1);
        return ((funcptr_unlinkat)libc_unlinkat)(dirfd, passpath, flags);
    }

    int rmdir(const char *pathname){
        char passpath[PATH_MAX];
        init_path("rmdir", pathname, passpath, 0);
        return ((funcptr_rmdir)libc_rmdir)(passpath);
    }

    int access(const char *pathname, int mode){
        char passpath[PATH_MAX];
        init_path("access", pathname, passpath, 0);
        return ((funcptr_access)libc_access)(passpath, mode);
    }

    int faccessat(int dirfd, const char *pathname, int mode, int flags){
        char passpath[PATH_MAX];
        init_path("faccessat", pathname, passpath, 0, 1);
        return ((funcptr_faccessat)libc_faccessat)(dirfd, passpath, mode, flags);
    }

    int stat(const char *pathname, struct stat *statbuf){
        char passpath[PATH_MAX];
        init_path("stat", pathname, passpath, 0);
        return ((funcptr_stat)libc_stat)(passpath, statbuf);
    }

    int lstat(const char *pathname, struct stat *statbuf){
        char passpath[PATH_MAX];
        init_path("lstat", pathname, passpath, 0);
        return ((funcptr_lstat)libc_lstat)(passpath, statbuf);
    }

    int lstat64(const char *pathname, struct stat64 *statbuf){
        char passpath[PATH_MAX];
        init_path("lstat64", pathname, passpath, 0);
        return ((funcptr_lstat64)libc_lstat64)(passpath, statbuf);
    }

    int fstat(int fd, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fstat %d", fd);
        return ((funcptr_fstat)libc_fstat)(fd, statbuf);
    }

    int fstatat(int dirfd, char const *path, struct stat *statbuf, int flags){
        char passpath[PATH_MAX];
        init_path("fstatat", path, passpath, 0, 1);
        return ((funcptr_fstatat)libc_fstatat)(dirfd, passpath, statbuf, flags);
    }

    int statvfs(const char *path, struct statvfs *buf){
        char passpath[PATH_MAX];
        init_path("statvfs", path, passpath, 0);
        return ((funcptr_statvfs)libc_statvfs)(passpath, buf);
    }

    int __xstat(int ver, const char *path, struct stat *statbuf){
        char passpath[PATH_MAX];
        init_path("__xstat", path, passpath, 0);
        return ((funcptr___xstat)libc___xstat)(ver, passpath, statbuf);
    }

    int _xstat(int ver, const char *path, struct stat *statbuf){
        char passpath[PATH_MAX];
        init_path("_xstat", path, passpath, 0);
        return ((funcptr__xstat)libc__xstat)(ver, passpath, statbuf);
    }

    int __xstat64(int ver, const char *path, struct stat64 *statbuf){
        char passpath[PATH_MAX];
        init_path("__xstat64", path, passpath, 0);
        return ((funcptr___xstat64)libc___xstat64)(ver, passpath, statbuf);
    }

    int __fxstat(int ver, int fd, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fxstat %d", fd);
        return ((funcptr___fxstat)libc___fxstat)(ver, fd, statbuf);
    }

    int __fxstatat(int ver, int fd, const char *path, struct stat *statbuf, int flag){
        char passpath[PATH_MAX];
        init_path("__fxstatat", path, passpath, 0, 1);
        return ((funcptr___fxstatat)libc___fxstatat)(ver, fd, passpath, statbuf, flag);
    }

    int __fxstat64(int ver, int fd, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fxstat64 %d", fd);
        return ((funcptr___fxstat64)libc___fxstat64)(ver, fd, statbuf);
    }

    int __fxstatat64(int ver, int fd, const char *path, struct stat64 *statbuf, int flag) {
        char passpath[PATH_MAX];
        init_path("__fxstatat64", path, passpath, 0);
        return ((funcptr___fxstatat64)libc___fxstatat64)(ver, fd, passpath, statbuf, flag);
    }

    int __lxstat(int ver, const char *path, struct stat *statbuf){
        char passpath[PATH_MAX];
        init_path("__lxstat", path, passpath, 0, 1);
        return ((funcptr___lxstat)libc___lxstat)(ver, passpath, statbuf);
    }

    int __lxstat64(int ver, const char *path, struct stat64 *statbuf){
        char passpath[PATH_MAX];
        init_path("__lxstat64", path, passpath, 0);
        return ((funcptr___lxstat64)libc___lxstat64)(ver, passpath, statbuf);
    }

    int statfs(const char *path, struct statfs *buf){
        char passpath[PATH_MAX];
        init_path("statfs", path, passpath, 0);
        return ((funcptr_statfs)libc_statfs)(passpath, buf);
    }

    //TODO: this function might need some fixing
    FILE* fopen(const char *path, const char *mode){
        initialize_passthrough_if_necessary();
        if(!strcmp(path, get_config_file()))
        {
            // Disable functions because config parsing requires fopen
            initialize_functions();
            FILE * f = ((funcptr_fopen)libc_fopen)(path, mode);
            return f;
        }
        char passpath[PATH_MAX];
        init_path("fopen", path, passpath, 0);
        return ((funcptr_fopen)libc_fopen)(passpath, mode);
    }

    FILE* fopen64(const char *path, const char *mode){
        char passpath[PATH_MAX];
        init_path("fopen64", path, passpath, 0);
        return ((funcptr_fopen64)libc_fopen64)(passpath, mode);
    }

    FILE* freopen(const char *path, const char *mode, FILE *stream){
        char passpath[PATH_MAX];
        init_path("freopen", path, passpath, 0);
        return ((funcptr_freopen)libc_freopen)(passpath, mode, stream);
    }

    FILE* freopen64(const char *path, const char *mode, FILE *stream){
        char passpath[PATH_MAX];
        init_path("freopen64", path, passpath, 0);
        return ((funcptr_freopen64)libc_freopen64)(passpath, mode, stream);
    }


    int truncate(const char *path, off_t offset){
        char passpath[PATH_MAX];
        init_path("truncate", path, passpath, 0);
        return ((funcptr_truncate)libc_truncate)(passpath, offset);
    }

    //int truncate64(const char *path, off64_t offset){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(path, passpath);
    //    log_msg(INFO, "truncate64 %s", passpath);
    //    return ((funcptr_truncate64)libc_truncate64)(passpath, offset);
    //}

    int ftruncate(int fd, off_t offset){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "ftruncate %d", fd);
        return ((funcptr_ftruncate)libc_ftruncate)(fd, offset);
    }

    int chown(const char *pathname, uid_t owner, gid_t group){
        char passpath[PATH_MAX];
        init_path("chown", pathname, passpath, 0);
        return ((funcptr_chown)libc_chown)(passpath, owner, group);
    }

    int lchown(const char *pathname, uid_t owner, gid_t group){
        char passpath[PATH_MAX];
        init_path("lchown", pathname, passpath, 0);
        return ((funcptr_lchown)libc_lchown)(passpath, owner, group);
    }

    int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags){
        char passpath[PATH_MAX];
        init_path("fchownat", pathname, passpath, 0, 1);
        return ((funcptr_fchownat)libc_fchownat)(dirfd, passpath, owner, group, flags);
    }

    int chmod(const char *pathname, mode_t mode){
        char passpath[PATH_MAX];
        init_path("chmod", pathname, passpath, 0);
        return ((funcptr_chmod)libc_chmod)(passpath, mode);
    }

    int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags){
        char passpath[PATH_MAX];
        init_path("fchmodat", pathname, passpath, 0, 1);
        return ((funcptr_fchmodat)libc_fchmodat)(dirfd, passpath, mode, flags);
    }

    int removexattr(const char *path, const char *name){
        char passpath[PATH_MAX];
        init_path("removexattr", path, passpath, 0);
        return ((funcptr_removexattr)libc_removexattr)(passpath, name);
    }

    int lremovexattr(const char *path, const char *name){
        char passpath[PATH_MAX];
        init_path("lremovexattr", path, passpath, 0);
        return ((funcptr_lremovexattr)libc_lremovexattr)(passpath, name);
    }

    ssize_t listxattr(const char *path, char *list, size_t size){
        char passpath[PATH_MAX];
        init_path("listxattr", path, passpath, 0);
        return ((funcptr_listxattr)libc_listxattr)(passpath, list, size);
    }

    ssize_t llistxattr(const char *path, char *list, size_t size){
        char passpath[PATH_MAX];
        init_path("llistxattr", path, passpath, 0);
        return ((funcptr_llistxattr)libc_llistxattr)(passpath, list, size);
    }

    ssize_t getxattr(const char *path, const char *name,
                        void *value, size_t size){
        char passpath[PATH_MAX];
        init_path("getxattr", path, passpath, 0);
        return ((funcptr_lgetxattr)libc_getxattr)(passpath, name, value, size);
    }

    ssize_t lgetxattr(const char *path, const char *name,
                        void *value, size_t size){
        char passpath[PATH_MAX];
        init_path("lgetxattr", path, passpath, 0);
        return ((funcptr_lgetxattr)libc_lgetxattr)(passpath, name, value, size);
    }

    int setxattr(const char* path, const char *name, const void *value, size_t size, int flags){
        char passpath[PATH_MAX];
        init_path("setxattr", path, passpath, 0);
        return ((funcptr_setxattr)libattr_setxattr)(passpath, name, value, size, flags);
    }

    int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fsetxattr %d", fd);
        return ((funcptr_fsetxattr)libattr_fsetxattr)(fd, name, value, size, flags);
    }

    const char* magic_file(magic_t cookie, const char *filename){
        char passpath[PATH_MAX];
        init_path("magic_file", filename, passpath, 0);
        return ((funcptr_magic_file)libmagic_magic_file)(cookie, passpath);
    }

    int euidaccess(const char *pathname, int mode){
        char passpath[PATH_MAX];
        init_path("euidaccess", pathname, passpath, 0);
        return ((funcptr_euidaccess)libc_euidaccess)(passpath, mode);
    }
    int eaccess(const char *pathname, int mode){
        char passpath[PATH_MAX];
        init_path("eaccess", pathname, passpath, 0);
        return ((funcptr_eaccess)libc_eaccess)(passpath, mode);
    }

    void copy_last6(char* templ, char* passpath, int suffixlen)
    {
        // Copy the last 6 characters of passpath into the 
        // last 6 characters of templ. Useful to copy the result 
        // of pattern resolution (e.g., sedXXXXXX -> sed123456) in
        // mk*temp calls. If suffixlen is not 0, skips the last suffixlen
        // characters: for instance, the Xs in sedXXXXXX123 would be reached
        // with a suffixlen of 3.
        int len_temp = strlen(templ);
        int len_pass = strlen(passpath);
        for(int i = 0; i < 6; i++)
        {
            templ[len_temp-i-1-suffixlen]=passpath[len_pass-i-1-suffixlen];
        }
    }

    int mkstemp(char * templ){
        char passpath[PATH_MAX];
        init_path("mkstemp", templ, passpath, 0);
        int ret = ((funcptr_mkstemp)libc_mkstemp)(passpath);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkstemp64(char * templ){
        char passpath[PATH_MAX];
        init_path("mkstemp64", templ, passpath, 0);
        int ret = ((funcptr_mkstemp64)libc_mkstemp64)(passpath);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkostemp(char * templ, int flags){
        char passpath[PATH_MAX];
        init_path("mkostemp", templ, passpath, 0);
        int ret = ((funcptr_mkostemp)libc_mkostemp)(passpath, flags);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkostemp64(char * templ, int flags){
        char passpath[PATH_MAX];
        init_path("mkostemp64", templ, passpath, 0);
        templ = &passpath[0];
        int ret = ((funcptr_mkostemp64)libc_mkostemp64)(templ, flags);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkstemps(char * templ, int suffixlen){
        char passpath[PATH_MAX];
        init_path("mkstemps", templ, passpath, 0);
        int ret = ((funcptr_mkstemps)libc_mkstemps)(passpath, suffixlen);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkstemps64(char * templ, int suffixlen){
        char passpath[PATH_MAX];
        init_path("mkstemps64", templ, passpath, 0);
        int ret = ((funcptr_mkstemps64)libc_mkstemps64)(passpath, suffixlen);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkostemps(char * templ, int suffixlen, int flags){
        char passpath[PATH_MAX];
        init_path("mkostemps", templ, passpath, 0);
        int ret = ((funcptr_mkostemps)libc_mkostemps)(passpath, suffixlen, flags);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkostemps64(char * templ, int suffixlen, int flags){
        char passpath[PATH_MAX];
        init_path("mkostemps64", templ, passpath, 0);
        int ret = ((funcptr_mkostemps64)libc_mkostemps64)(passpath, suffixlen, flags);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }

    FILE * setmntent (const char *file, const char *mode){
        char passpath[PATH_MAX];
        init_path("setmntent", file, passpath, 0);
        return ((funcptr_setmntent)libc_setmntent)(passpath, mode);

    }

    char * bindtextdomain (const char *domainname, const char *dirname){
        char passpath[PATH_MAX];
        init_path("bindtextdomain", dirname, passpath, 0);
        return ((funcptr_bindtextdomain)libc_bindtextdomain)(domainname, passpath);
    }

    int symlink (const char *oldname, const char *newname){
        char old_passpath[PATH_MAX];
        char new_passpath[PATH_MAX];
        init_twopaths("symlink", oldname, newname, old_passpath, new_passpath, 0);
        return ((funcptr_symlink)libc_symlink)(old_passpath, new_passpath);
    }

    ssize_t readlink (const char *filename, char *buffer, size_t size){
        char passpath[PATH_MAX];
        init_path("readlink", filename, passpath, 0);
        return ((funcptr_readlink)libc_readlink)(passpath, buffer, size);
    }

    int nftw(const char *dirpath,
             int (*fn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf),
             int nopenfd, int flags){

        char passpath[PATH_MAX];
        init_path("nftw", dirpath, passpath, 0);
        return ((funcptr_nftw)libc_nftw)(passpath, fn, nopenfd, flags);
    }

    int ftw(const char *dirpath,
            int (*fn) (const char *fpath, const struct stat *sb, int typeflag),
            int nopenfd){
        char passpath[PATH_MAX];
        init_path("ftw", dirpath, passpath, 0);
        return ((funcptr_ftw)libc_ftw)(passpath, fn, nopenfd);
    }

    int name_to_handle_at(int dirfd, const char *pathname, struct file_handle *handle, int *mount_id, int flags){
        char passpath[PATH_MAX];
        init_path("name_to_handle_at", pathname, passpath, 0, 1);
        return ((funcptr_name_to_handle_at)libc_name_to_handle_at)(dirfd, passpath, handle, mount_id, flags);
    }

    int chroot(const char *path){
        char passpath[PATH_MAX];
        init_path("chroot", path, passpath, 0);
        return ((funcptr_chroot)libc_chroot)(passpath);
    }

    int execve(const char *pathname, char *const argv[], char *const envp[]){
        char passpath[PATH_MAX];
        init_path("execve", pathname, passpath, 0);
        return ((funcptr_execve)libc_execve)(passpath, argv, envp);
    }

    long int pathconf (const char *filename, int parameter){
        char passpath[PATH_MAX];
        init_path("pathconf", filename, passpath, 0);
        return ((funcptr_pathconf)libc_pathconf)(passpath, parameter);
    }

    char* tempnam(const char *dir, const char *prefix){
        char passpath[PATH_MAX];
        init_path("tempnam", dir, passpath, 0);
        return ((funcptr_tempnam)libc_tempnam)(passpath, prefix);

    }

    int mkfifo(const char *path, mode_t mode){
        char passpath[PATH_MAX];
        init_path("mkfifo", path, passpath, 0);
        return ((funcptr_mkfifo)libc_mkfifo)(passpath, mode);
    }

    char* realpath(const char *path, char *resolved_path){
        char passpath[PATH_MAX];
        init_path("realpath", path, passpath, 1);
        return ((funcptr_realpath)libc_realpath)(passpath, resolved_path);
    }

    char* canonicalize_file_name (const char *name){
        char passpath[PATH_MAX];
        init_path("canonicalize_file_name", name, passpath, 1);
        return ((funcptr_canonicalize_file_name)libc_canonicalize_file_name)(passpath);
    }

    char* getcwd(char *buffer, size_t size){
       
        initialize_passthrough_if_necessary();
        char* path;
        char passpath[PATH_MAX];
        path = ((funcptr_getwd)libc_getcwd)(buffer, size);
        // get masked path rather than real path
        init_path("getcwd", path, passpath, 1);
        // make sure there are no trailing characters after memmove
        if (strlen(path) > strlen(passpath))
            path[strlen(passpath)] = '\0';
        memmove(path, passpath, strlen(passpath));
        return path;
    }
    
    //int mknod(const char *path, mode_t mode, dev_t dev){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(path, passpath);
    //    log_msg(INFO, "mknod %s", passpath);
    //    return ((funcptr_mknod)libc_mknod)(passpath, mode, dev);
    //}
    

    // removed as undefined
    //int execveat(int dirfd, const char *pathname,
    //                char *const argv[], char *const envp[],
    //                int flags){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(pathname, passpath);
    //    log_msg(INFO, "execveat %s", passpath);
    //    return ((funcptr_execveat)libc_execveat)(dirfd, passpath, argv, envp, flags);
    //}

    //int fanotify_mark(int fanotify_fd, unsigned int flags,
    //                     uint64_t mask, int dirfd, const char *pathname){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(pathname, passpath);
    //    log_msg(INFO, "fanotify_mark %s", passpath);
    //    return ((funcptr_fanotify_mark)libc_fanotify_mark)(fanotify_fd, flags, mask, dirfd, passpath);
    //}

}

// Note: don't really need but keeping it here just in case
//char* dirname(char* path){
//    initialize_passthrough_if_necessary();
//    char passpath[PATH_MAX];
//    pass_getpath(path, passpath);
//    log_msg(INFO, "dirname %s", passpath);
//    return ((funcptr_dirname)libc_dirname)(passpath);
//}

// Note: don't really need but keeping it here just in case
//const char* basename (const char *filename) throw() {
//    initialize_passthrough_if_necessary();
//    char passpath[PATH_MAX];
//    pass_getpath(filename, passpath);
//    log_msg(INFO, "basename %s", passpath);
//    return ((funcptr_basename)libc_basename)(passpath);
//}

