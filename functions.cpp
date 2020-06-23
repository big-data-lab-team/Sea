#include <string.h>
#include "functions.h"
#include "logger.h"
#include "config.h"
#include <stdlib.h>

extern "C" {

    int open(__const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else 
            pass_getpath(pathname, passpath, 0);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        log_msg(INFO, "open %s", pathname);
        return ((funcptr_open)libc_open)(passpath, flags, mode);
    }

    int __open(__const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        log_msg(INFO, "__open %s", pathname);
        return ((funcptr___open)libc___open)(passpath, flags, mode);
    }

    int __open_2 (const char *file, int oflag){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(file, passpath, 0);
        }
        else
            pass_getpath(file, passpath, 0);


        log_msg(INFO, "__open_2 %s", file);
        return ((funcptr___open_2)libc___open_2)(passpath, oflag);

    }

    int open64(const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        log_msg(INFO, "open64 %s", pathname);
        return ((funcptr_open64)libc_open64)(passpath, flags, mode);
    }

    int __open64(__const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        log_msg(INFO, "open64 %s", pathname);
        return ((funcptr___open64)libc___open64)(passpath, flags, mode);
    }

    int openat(int dirfd, const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("openat %s\n", pathname);
        int match;
        struct config sea_conf = get_sea_config();

        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "openat file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int openat64(int dirfd, const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("openat64 %s\n", pathname);
        int match;
        struct config sea_conf = get_sea_config();

        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "openat64 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int __openat64_2(int dirfd, const char* pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("openat64_2 %s\n", pathname);
        int match;
        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "openat64_2 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    extern int __openat_2(int dirfd, const char* pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("openat_2 %s\n", pathname);
        int match;
        struct config sea_conf = get_sea_config();

        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "openat_2 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int openat2(int dirfd, const char *pathname,
                   struct open_how *how, size_t size){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("openat2 %s\n", pathname);
        int match;
        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "openat2 file %s", passpath);
        return ((funcptr_openat2)libc_openat2)(dirfd, passpath, how, size);
    }

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
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(dirp, passpath, 0);
        }
        else
            pass_getpath(dirp, passpath, 0);

        log_msg(INFO, "scandir %s", passpath);
        return ((funcptr_scandir)libc_scandir)(passpath, namelist, filter, compar);
    }

    int scandir64(const char *dirp, struct dirent64 ***namelist,
                  int (*select)(const struct dirent64 *),
                  int (*cmp)(const struct dirent64 **, const struct dirent64 **)){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(dirp, passpath, 0);
        }
        else
            pass_getpath(dirp, passpath, 0);

        log_msg(INFO, "scandir64 %s", passpath);
        return ((funcptr_scandir64)libc_scandir64)(passpath, namelist, select, cmp);
    }

    int scandirat (int dfd, const char *dir, struct dirent ***namelist,
             int (*select) (const struct dirent *),
             int (*cmp) (const struct dirent **, const struct dirent **))
    {
        initialize_passthrough_if_necessary();
        log_msg(INFO, "scandirat test");
        return 0;
    }

    int scandirat64 (int dfd, const char *dir, struct dirent64 ***namelist,
             int (*select) (const struct dirent64 *),
             int (*cmp) (const struct dirent64 **, const struct dirent64 **))
    {
        initialize_passthrough_if_necessary();
        log_msg(INFO, "scandir64 test");
        return 0;
    }

    struct dirent *sea_readnext(struct dirent* d, config sea_conf, SEA_DIR* sd) { 
        int found = 0;
        if (d != NULL && d->d_name[0] != '\0') {
            for (int i = 0 ; i < sea_conf.n_sources; ++i) {
                if (i != sd->curr_index + 1) {
                    char tmppath[PATH_MAX];
                    strcpy(tmppath, sea_conf.source_mounts[i]);

                    if (sea_conf.source_mounts[i][strlen(sea_conf.source_mounts[i]) - 1] != '/')
                        strcat(tmppath, "/");

                    strcat(tmppath, d->d_name);

                    // no way around using the vector here 
                    for (auto file: sea_files) {
                        //printf("file %s tmppath %s\n", file, tmppath);
                        if (strcmp(file, tmppath) == 0) {
                            found = 1;
                            break;
                        }
                    }

                    if (found)
                        break;
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
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
            //printf("makedir sea %s\n", passpath);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "mkdir path %s", passpath);
        return ((funcptr_mkdir)libc_mkdir)(passpath, mode);
    }

    int chdir(const char *pathname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "chdir path %s", passpath);
        return ((funcptr_chdir)libc_chdir)(passpath);
    }

    int rename(const char *oldpath, const char *newpath){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(oldpath, oldpasspath, 0);
            sea_getpath(newpath, newpasspath, 0);
        }
        else {
            pass_getpath(oldpath, oldpasspath, 0);
            pass_getpath(newpath, newpasspath, 0);
        }

        log_msg(INFO, "rename file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_rename)libc_rename)(oldpasspath, newpasspath);
    }

    int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(oldpath, oldpasspath, 0);
            sea_getpath(newpath, newpasspath, 0);
        }
        else {
            pass_getpath(oldpath, oldpasspath, 0);
            pass_getpath(newpath, newpasspath, 0);
        }

        log_msg(INFO, "renameat file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_renameat)libc_renameat)(olddirfd, oldpasspath, newdirfd, newpasspath);
    }

    int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(oldpath, oldpasspath, 0);
            sea_getpath(newpath, newpasspath, 0);
        }
        else {
            pass_getpath(oldpath, oldpasspath, 0);
            pass_getpath(newpath, newpasspath, 0);
        }

        log_msg(INFO, "renameat2 file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_renameat2)libc_renameat2)(olddirfd, oldpasspath, newdirfd, newpasspath, flags);
    }

    int remove(const char *pathname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "remove file %s", passpath);
        return ((funcptr_remove)libc_remove)(passpath);
    }

    int link (const char *oldname, const char *newname){
        initialize_passthrough_if_necessary();
        char old_passpath[PATH_MAX];
        char new_passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(oldname, old_passpath, 0);
            sea_getpath(newname, new_passpath, 0);
        }
        else {
            pass_getpath(oldname, old_passpath, 0);
            pass_getpath(newname, new_passpath, 0);

        }

        log_msg(INFO, "link file %s %s", old_passpath, new_passpath);
        return ((funcptr_link)libc_link)(old_passpath, new_passpath);
    }

    int linkat (int oldfd, const char *oldname, int newfd, const char *newname, int flags){
        initialize_passthrough_if_necessary();
        char old_passpath[PATH_MAX];
        char new_passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(oldname, old_passpath, 0);
            sea_getpath(newname, new_passpath, 0);
        }
        else {
            pass_getpath(oldname, old_passpath, 0);
            pass_getpath(newname, new_passpath, 0);

        }

        log_msg(INFO, "linkat file %s %s", old_passpath, new_passpath);
        return ((funcptr_linkat)libc_linkat)(oldfd, old_passpath, newfd, new_passpath, flags);

    }

    int unlink(const char *pathname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "unlink file %s", passpath);
        return ((funcptr_unlink)libc_unlink)(passpath);
    }

    int unlinkat(int dirfd, const char *pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        log_msg(INFO, "entering unlinkat %s", pathname);
        struct config sea_conf = get_sea_config();
        int match;

        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "completing unlinkat %s", passpath);
        return ((funcptr_unlinkat)libc_unlinkat)(dirfd, passpath, flags);
    }

    int rmdir(const char *pathname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "rmdir %s", passpath);
        return ((funcptr_rmdir)libc_rmdir)(passpath);
    }

    int access(const char *pathname, int mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "access path %s", pathname);
        return ((funcptr_access)libc_access)(passpath, mode);
    }

    int faccessat(int dirfd, const char *pathname, int mode, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        struct config sea_conf = get_sea_config();
        int match;

        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "faccessat path %s", passpath);
        return ((funcptr_faccessat)libc_faccessat)(dirfd, passpath, mode, flags);
    }

    int stat(const char *pathname, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("stat %s\n", pathname);
        
        if (!get_internal()) {
            log_msg(INFO, "entering stat %s", pathname);

            struct config sea_conf = get_sea_config();
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(pathname, passpath, 0);
            }
            else
                pass_getpath(pathname, passpath, 0);

            log_msg(INFO, "completed stat %s", passpath);
        }
        else {
            strcpy(passpath, pathname);
        }
        return ((funcptr_stat)libc_stat)(passpath, statbuf);
    }

    int lstat(const char *pathname, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("lstat %s\n", pathname);
        if (!get_internal()) {
            log_msg(INFO, "entering lstat %s", pathname);

            struct config sea_conf = get_sea_config();
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(pathname, passpath, 0);
            }
            else
                pass_getpath(pathname, passpath, 0);

            log_msg(INFO, "completed lstat %s", passpath);
        }
        else {
            strcpy(passpath, pathname);
        }
        //printf("return lstat %d\n", get_internal());
        return ((funcptr_lstat)libc_lstat)(passpath, statbuf);
    }

    int lstat64(const char *pathname, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("lstat64 %s\n", pathname);
        
        if (!get_internal()) {
            log_msg(INFO, "entering lstat64 %s", pathname);

            struct config sea_conf = get_sea_config();
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(pathname, passpath, 0);
            }
            else
                pass_getpath(pathname, passpath, 0);

            log_msg(INFO, "completed lstat64 %s", passpath);
        }
        else {
            strcpy(passpath, pathname);
        }
        return ((funcptr_lstat64)libc_lstat64)(passpath, statbuf);
    }

    int fstat(int fd, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fstat");
        return ((funcptr_fstat)libc_fstat)(fd, statbuf);
    }

    int fstatat(int dirfd, char const *path, struct stat *statbuf, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("fstatat %s\n", path);
        
        if (!get_internal()) {
            int match;
            log_msg(INFO, "entering fstatat %s", path);

            struct config sea_conf = get_sea_config();
        
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                match = sea_getpath(path, passpath, 0);
            }
            else
                match = pass_getpath(path, passpath, 0);


            if (match == 0) {
                strcpy(passpath, path);
            }
        }

        else { 
            strcpy(passpath, path);
        }

        log_msg(INFO, "completed fstatat %s", passpath);
        return ((funcptr_fstatat)libc_fstatat)(dirfd, passpath, statbuf, flags);
    }

    int statvfs(const char *path, struct statvfs *buf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("statvfs %s\n", path);

        if (!get_internal()) {
            log_msg(INFO, "entering statvfs %s", path);

            struct config sea_conf = get_sea_config();
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(path, passpath, 0);
            }
            else
                pass_getpath(path, passpath, 0);
            log_msg(INFO, "completed statvfs %s", passpath);
        }
        else {
            strcpy(passpath, path);
        }

        return ((funcptr_statvfs)libc_statvfs)(passpath, buf);
    }

    int __xstat(int ver, const char *path, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("__xstat %s\n", path);
        if (!get_internal()) {
            log_msg(INFO, "entering xstat %s", path);

            struct config sea_conf = get_sea_config();

            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(path, passpath, 0);
            }
            else
                pass_getpath(path, passpath, 0);

            log_msg(INFO, "completed xstat %s", passpath);
        }
        else {
            strcpy(passpath, path);
        }
        return ((funcptr___xstat)libc___xstat)(ver, passpath, statbuf);
    }

    int _xstat(int ver, const char *path, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("_xstat %s\n", path);

        if (!get_internal()) {
            log_msg(INFO, "entering xstat %s", path);
            struct config sea_conf = get_sea_config();
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(path, passpath, 0);
            }
            else
                pass_getpath(path, passpath, 0);

            log_msg(INFO, "completed xstat %s", passpath);
        }
        else {
            strcpy(passpath, path);
        }
        //printf("__xstat %s", passpath);
        return ((funcptr__xstat)libc__xstat)(ver, passpath, statbuf);
    }

    int __xstat64(int ver, const char *path, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        //printf("__xstat64 %s\n", path);

        if (!get_internal()) {
            struct config sea_conf = get_sea_config();
            log_msg(INFO, "entering xstat64 %s", path);
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(path, passpath, 0);
            }
            else
                pass_getpath(path, passpath, 0);

            log_msg(INFO, "completed xstat64 %s", passpath);
        }
        else {
            strcpy(passpath, path);
        }
        return ((funcptr___xstat64)libc___xstat64)(ver, passpath, statbuf);
    }

    int __fxstat(int ver, int fd, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fxstat");
        return ((funcptr___fxstat)libc___fxstat)(ver, fd, statbuf);
    }

    int __fxstatat(int ver, int fd, const char *path, struct stat *statbuf, int flag){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        if (!get_internal()) {
            int match;

            struct config sea_conf = get_sea_config();
            log_msg(INFO, "entering fxstatat %s", path);
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                match = sea_getpath(path, passpath, 0);
            }
            else
                match = pass_getpath(path, passpath, 0);

            if (match == 0) {
                strcpy(passpath, path);
            }

            log_msg(INFO, "completed fxstatat %s", passpath);
        }
        else {
            strcpy(passpath, path);
        }
        return ((funcptr___fxstatat)libc___fxstatat)(ver, fd, passpath, statbuf, flag);
    }

    int __fxstat64(int ver, int fd, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fxstat64");
        return ((funcptr___fxstat64)libc___fxstat64)(ver, fd, statbuf);
    }

    int __fxstatat64(int ver, int fd, const char *path, struct stat64 *statbuf, int flag) {
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        if (!get_internal()) { 
            int match;
            struct config sea_conf = get_sea_config();

            log_msg(INFO, "entering fxstatat64 %s", path);
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                match = sea_getpath(path, passpath, 0);
            }
            else {
                match = pass_getpath(path, passpath, 0);
            }

            if (match == 0) {
                strcpy(passpath, path);
            }

            log_msg(INFO, "completed fxstatat64 %s", passpath);
        }
        else {
            strcpy(passpath, path);
        }
        return ((funcptr___fxstatat64)libc___fxstatat64)(ver, fd, passpath, statbuf, flag);
    }

    int __lxstat(int ver, const char *path, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        //printf("lxstat %s %d\n", path, get_internal());
        if (!get_internal()) {
            log_msg(INFO, "entering lxstat %s", path);
            struct config sea_conf = get_sea_config();
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(path, passpath, 0);
            }
            else
                pass_getpath(path, passpath, 0);

            log_msg(INFO, "completed lxstat %s", passpath);
        }
        else {
            strcpy(passpath, path);
        }
        //printf("lxstat %s\n", passpath);
        return ((funcptr___lxstat)libc___lxstat)(ver, passpath, statbuf);
    }

    int __lxstat64(int ver, const char *path, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        if (!get_internal()) {
            log_msg(INFO, "entering lxstat64");
            struct config sea_conf = get_sea_config();
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(path, passpath, 0);
            }
            else
                pass_getpath(path, passpath, 0);

            log_msg(INFO, "completed lxstat64 %s", path);
        }
        else {
            strcpy(passpath, path);
        }
        return ((funcptr___lxstat64)libc___lxstat64)(ver, passpath, statbuf);
    }

    int statfs(const char *path, struct statfs *buf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        if (!get_internal()) {
            struct config sea_conf = get_sea_config();
            if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
                initialize_sea_if_necessary();
                sea_getpath(path, passpath, 0);
            }
            else
                pass_getpath(path, passpath, 0);

            log_msg(INFO, "statfs %s", passpath);
        }
        else {
            strcpy(passpath, path);
        }

        return ((funcptr_statfs)libc_statfs)(passpath, buf);
    }

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

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "fopen %s mode %s", path, mode);
        return ((funcptr_fopen)libc_fopen)(passpath, mode);
    }

    FILE* fopen64(const char *path, const char *mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "fopen %s %s", passpath, mode);
        return ((funcptr_fopen64)libc_fopen64)(passpath, mode);
    }

    FILE* freopen(const char *path, const char *mode, FILE *stream){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "freopen %s %s", passpath, mode);
        return ((funcptr_freopen)libc_freopen)(passpath, mode, stream);
    }

    FILE* freopen64(const char *path, const char *mode, FILE *stream){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "freopen64 %s %s", passpath, mode);
        return ((funcptr_freopen64)libc_freopen64)(passpath, mode, stream);
    }


    int truncate(const char *path, off_t offset){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "truncate %s", passpath);
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
        log_msg(INFO, "ftruncate");
        return ((funcptr_ftruncate)libc_ftruncate)(fd, offset);
    }

    int chown(const char *pathname, uid_t owner, gid_t group){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "chown %s", passpath);
        return ((funcptr_chown)libc_chown)(passpath, owner, group);
    }

    int lchown(const char *pathname, uid_t owner, gid_t group){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "lchown %s", passpath);
        return ((funcptr_lchown)libc_lchown)(passpath, owner, group);
    }

    int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        int match = 0;
        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "fchownat %s", passpath);
        return ((funcptr_fchownat)libc_fchownat)(dirfd, passpath, owner, group, flags);
    }

    int chmod(const char *pathname, mode_t mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "chmod %s", passpath);
        return ((funcptr_chmod)libc_chmod)(passpath, mode);
    }

    int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        int match;
        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            match = sea_getpath(pathname, passpath, 0);
        }
        else
            match = pass_getpath(pathname, passpath, 0);

        if (match == 0) {
            strcpy(passpath, pathname);
        }

        log_msg(INFO, "fchmodat %s", passpath);
        return ((funcptr_fchmodat)libc_fchmodat)(dirfd, passpath, mode, flags);
    }

    int removexattr(const char *path, const char *name){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "removexattr %s", passpath);
        return ((funcptr_removexattr)libc_removexattr)(passpath, name);
    }

    int lremovexattr(const char *path, const char *name){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "lremovexattr %s", passpath);
        return ((funcptr_lremovexattr)libc_lremovexattr)(passpath, name);
    }

    ssize_t listxattr(const char *path, char *list, size_t size){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "listxattr %s", passpath);
        return ((funcptr_listxattr)libc_listxattr)(passpath, list, size);
    }

    ssize_t llistxattr(const char *path, char *list, size_t size){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "llistxattr %s", passpath);
        return ((funcptr_llistxattr)libc_llistxattr)(passpath, list, size);
    }

    ssize_t getxattr(const char *path, const char *name,
                        void *value, size_t size){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "getxattr %s", passpath);
        return ((funcptr_lgetxattr)libc_getxattr)(passpath, name, value, size);
    }

    ssize_t lgetxattr(const char *path, const char *name,
                        void *value, size_t size){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "lgetxattr %s", passpath);
        return ((funcptr_lgetxattr)libc_lgetxattr)(passpath, name, value, size);
    }

    int setxattr(const char* path, const char *name, const void *value, size_t size, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "setxattr %s", path);
        return ((funcptr_setxattr)libattr_setxattr)(passpath, name, value, size, flags);
    }

    int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fsetxattr");
        return ((funcptr_fsetxattr)libattr_fsetxattr)(fd, name, value, size, flags);
    }

    const char* magic_file(magic_t cookie, const char *filename){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(filename, passpath, 0);
        }
        else
            pass_getpath(filename, passpath, 0);

        log_msg(INFO, "magic_file");
        return ((funcptr_magic_file)libmagic_magic_file)(cookie, passpath);
    }

    int euidaccess(const char *pathname, int mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else 
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "euidaccess");
        return ((funcptr_euidaccess)libc_euidaccess)(passpath, mode);
    }
    int eaccess(const char *pathname, int mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "eaccess");
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
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(templ, passpath, 0);
        }
        else
            pass_getpath(templ, passpath, 0);

        log_msg(INFO, "mkstemp");
        int ret = ((funcptr_mkstemp)libc_mkstemp)(passpath);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkstemp64(char * templ){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(templ, passpath, 0);
        }
        else
            pass_getpath(templ, passpath, 0);

        log_msg(INFO, "mkstemp64");
        int ret = ((funcptr_mkstemp64)libc_mkstemp64)(passpath);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkostemp(char * templ, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(templ, passpath, 0);
        }
        else
            pass_getpath(templ, passpath, 0);

        log_msg(INFO, "mkostemp");
        int ret = ((funcptr_mkostemp)libc_mkostemp)(passpath, flags);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkostemp64(char * templ, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(templ, passpath, 0);
        }
        else
            pass_getpath(templ, passpath, 0);

        log_msg(INFO, "mkostemp64");
        templ = &passpath[0];
        int ret = ((funcptr_mkostemp64)libc_mkostemp64)(templ, flags);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkstemps(char * templ, int suffixlen){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(templ, passpath, 0);
        }
        else
            pass_getpath(templ, passpath, 0);

        log_msg(INFO, "mkstemps");
        int ret = ((funcptr_mkstemps)libc_mkstemps)(passpath, suffixlen);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkstemps64(char * templ, int suffixlen){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(templ, passpath, 0);
        }
        else
            pass_getpath(templ, passpath, 0);

        log_msg(INFO, "mkstempts64");
        int ret = ((funcptr_mkstemps64)libc_mkstemps64)(passpath, suffixlen);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkostemps(char * templ, int suffixlen, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(templ, passpath, 0);
        }
        else
            pass_getpath(templ, passpath, 0);

        log_msg(INFO, "mkostemps");
        int ret = ((funcptr_mkostemps)libc_mkostemps)(passpath, suffixlen, flags);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkostemps64(char * templ, int suffixlen, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(templ, passpath, 0);
        }
        else
            pass_getpath(templ, passpath, 0);

        log_msg(INFO, "mkostemps64 %s", passpath);
        int ret = ((funcptr_mkostemps64)libc_mkostemps64)(passpath, suffixlen, flags);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }

    FILE * setmntent (const char *file, const char *mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(file, passpath, 0);
        }
        else
            pass_getpath(file, passpath, 0);

        log_msg(INFO, "stmntent %s", passpath);
        return ((funcptr_setmntent)libc_setmntent)(passpath, mode);

    }

    char * bindtextdomain (const char *domainname, const char *dirname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(dirname, passpath, 0);
        }
        else
            pass_getpath(dirname, passpath, 0);

        log_msg(INFO, "bindtextdomain %s", passpath);
        return ((funcptr_bindtextdomain)libc_bindtextdomain)(domainname, dirname);
    }

    int symlink (const char *oldname, const char *newname){
        initialize_passthrough_if_necessary();
        char old_passpath[PATH_MAX];
        char new_passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(oldname, old_passpath, 0);
            sea_getpath(newname, new_passpath, 0);
        }
        else {
            pass_getpath(oldname, old_passpath, 0);
            pass_getpath(newname, new_passpath, 0);
        }

        log_msg(INFO, "symlink: old: %s new: %s", old_passpath, new_passpath);
        return ((funcptr_symlink)libc_symlink)(old_passpath, new_passpath);
    }

    ssize_t readlink (const char *filename, char *buffer, size_t size){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        log_msg(INFO, "entering readlink %s", filename);
        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(filename, passpath, 0);
            //printf("readlink %s\n", passpath);
        }
        else
            pass_getpath(filename, passpath, 0);

        log_msg(INFO, "completing readlink %s", passpath);
        return ((funcptr_readlink)libc_readlink)(passpath, buffer, size);
    }

    int nftw(const char *dirpath,
             int (*fn) (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf),
             int nopenfd, int flags){

        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(dirpath, passpath, 0);
        }
        else
            pass_getpath(dirpath, passpath, 0);

        log_msg(INFO, "nftw %s", passpath);
        return ((funcptr_nftw)libc_nftw)(passpath, fn, nopenfd, flags);
    }

    int ftw(const char *dirpath,
            int (*fn) (const char *fpath, const struct stat *sb, int typeflag),
            int nopenfd){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(dirpath, passpath, 0);
        }
        else
            pass_getpath(dirpath, passpath, 0);

        log_msg(INFO, "ftw %s", passpath);
        return ((funcptr_ftw)libc_ftw)(passpath, fn, nopenfd);
    }

    int name_to_handle_at(int dirfd, const char *pathname, struct file_handle *handle, int *mount_id, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "name_to_handle_at %s", passpath);
        return ((funcptr_name_to_handle_at)libc_name_to_handle_at)(dirfd, passpath, handle, mount_id, flags);
    }

    int chroot(const char *path){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "chroot %s", passpath);
        return ((funcptr_chroot)libc_chroot)(passpath);
    }

    int execve(const char *pathname, char *const argv[], char *const envp[]){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(pathname, passpath, 0);
        }
        else
            pass_getpath(pathname, passpath, 0);

        log_msg(INFO, "execve %s", passpath);
        return ((funcptr_execve)libc_execve)(passpath, argv, envp);
    }

    long int pathconf (const char *filename, int parameter){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(filename, passpath, 0);
        }
        else
            pass_getpath(filename, passpath, 0);

        log_msg(INFO, "pathconf %s", passpath);
        return ((funcptr_pathconf)libc_pathconf)(passpath, parameter);
    }

    char* tempnam(const char *dir, const char *prefix){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(dir, passpath, 0);
        }
        else
            pass_getpath(dir, passpath, 0);

        log_msg(INFO, "tempname %s", passpath);
        return ((funcptr_tempnam)libc_tempnam)(passpath, prefix);

    }

    int mkfifo(const char *path, mode_t mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 0);
        }
        else
            pass_getpath(path, passpath, 0);

        log_msg(INFO, "mkfifo %s", passpath);
        return ((funcptr_mkfifo)libc_mkfifo)(passpath, mode);
    }

    char* realpath(const char *path, char *resolved_path){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        // get masked path rather than real path

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 1);
        }
        else
            pass_getpath(path, passpath, 1);

        log_msg(INFO, "realpath %s", passpath);
        return ((funcptr_realpath)libc_realpath)(passpath, resolved_path);
    }

    char* canonicalize_file_name (const char *name){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        // get masked path rather than real path

        struct config sea_conf = get_sea_config();
        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(name, passpath, 1);
        }
        else
            pass_getpath(name, passpath, 1);

        log_msg(INFO, "canonicalize_file_name %s", passpath);
        return ((funcptr_canonicalize_file_name)libc_canonicalize_file_name)(passpath);
    }

    char* getcwd(char *buffer, size_t size){
       
        initialize_passthrough_if_necessary();
        char* path;
        char passpath[PATH_MAX];
        path = ((funcptr_getwd)libc_getcwd)(buffer, size);
        // get masked path rather than real path
        struct config sea_conf = get_sea_config();

        if (sea_conf.parsed == true && sea_conf.n_sources > 1) {
            initialize_sea_if_necessary();
            sea_getpath(path, passpath, 1);
        }
        else
            pass_getpath(path, passpath, 1);

        // make sure there are no trailing characters after memmove
        if (strlen(path) > strlen(passpath))
            path[strlen(passpath)] = '\0';
        memmove(path, passpath, strlen(passpath));
        log_msg(INFO, "getcwd %s", path);
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

