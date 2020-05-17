#include <string.h>
#include "functions.h"
#include "logger.h"
#include "config.h"

extern "C" {

    int open(__const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "opening file %s", pathname);
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        return ((funcptr_open)libc_open)(passpath, flags, mode);
    }

    int __open(__const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "opening file %s", pathname);
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        return ((funcptr___open)libc___open)(passpath, flags, mode);
    }

    int __open_2 (const char *file, int oflag){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "opening file %s", file);
        char passpath[PATH_MAX];
        pass_getpath(file, passpath);
        return ((funcptr___open_2)libc___open_2)(passpath, oflag);

    }

    int open64(const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "open64 %s", pathname);
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        return ((funcptr_open64)libc_open64)(passpath, flags, mode);
    }

    int __open64(__const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "open64 %s", pathname);
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        return ((funcptr___open64)libc___open64)(passpath, flags, mode);
    }

    int openat(int dirfd, const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "openat file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int openat64(int dirfd, const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "openat64 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int __openat64_2(int dirfd, const char* pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "openat64_2 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    extern int __openat_2(int dirfd, const char* pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "openat_2 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }



    DIR* opendir(const char* pathname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "opening directory %s", passpath);
        return ((funcptr_opendir)libc_opendir)(passpath);
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
        pass_getpath(dirp, passpath);
        log_msg(INFO, "scandir %s", passpath);
        return ((funcptr_scandir)libc_scandir)(passpath, namelist, filter, compar);
    }

    int scandir64(const char *dirp, struct dirent64 ***namelist,
                  int (*select)(const struct dirent64 *),
                  int (*cmp)(const struct dirent64 **, const struct dirent64 **)){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(dirp, passpath);
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

    struct dirent *readdir(DIR *dirp){

        initialize_passthrough_if_necessary();
        struct dirent *d;
        log_msg(INFO, "readdir");
        errno = 0;
        d = ((funcptr_readdir)libc_readdir)(dirp);

        if (d == NULL && errno)
            log_msg(ERROR, "reading dir %s", d->d_name);
        return d;
    }


#undef creat
    int creat(__const char *name, mode_t mode) {
      initialize_passthrough_if_necessary();
      char passpath[PATH_MAX];
      pass_getpath(name, passpath);
      log_msg(INFO, "creat %s", name);
      return ((funcptr_creat)libc_creat)(name, mode);
    }

    int creat64(__const char *name, mode_t mode) {
      initialize_passthrough_if_necessary();
      char passpath[PATH_MAX];
      pass_getpath(name, passpath);
      log_msg(INFO, "creat64 %s", name);
      return ((funcptr_creat64)libc_creat64)(name, mode);
    }

    int close(int fd){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "closing file");
        return ((funcptr_close)libc_close)(fd);
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
        pass_getpath(pathname, passpath);
        log_msg(INFO, "mkdir path %s", passpath);
        return ((funcptr_mkdir)libc_mkdir)(passpath, mode);
    }

    int chdir(const char *pathname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "chdir path %s", passpath);
        return ((funcptr_chdir)libc_chdir)(passpath);
    }

    int rename(const char *oldpath, const char *newpath){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        pass_getpath(oldpath, oldpasspath);
        pass_getpath(newpath, newpasspath);
        log_msg(INFO, "rename file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_rename)libc_rename)(oldpasspath, newpasspath);
    }

    int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        pass_getpath(oldpath, oldpasspath);
        pass_getpath(newpath, newpasspath);
        log_msg(INFO, "renameat file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_renameat)libc_renameat)(olddirfd, oldpasspath, newdirfd, newpasspath);
    }

    int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        pass_getpath(oldpath, oldpasspath);
        pass_getpath(newpath, newpasspath);
        log_msg(INFO, "renameat2 file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_renameat2)libc_renameat2)(olddirfd, oldpasspath, newdirfd, newpasspath, flags);
    }

    int remove(const char *pathname){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "remove file %s", pathname);
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        return ((funcptr_remove)libc_remove)(passpath);
    }

    int unlink(const char *pathname){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "unlink file %s", pathname);
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        return ((funcptr_unlink)libc_unlink)(passpath);
    }

    int unlinkat(int dirfd, const char *pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "unlinkat file %s", passpath);
        return ((funcptr_unlinkat)libc_unlinkat)(dirfd, passpath, flags);
    }

    int access(const char *pathname, int mode){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "access path %s", pathname);
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        return ((funcptr_access)libc_access)(passpath, mode);
    }

    int faccessat(int dirfd, const char *pathname, int mode, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "faccessat path %s", passpath);
        return ((funcptr_faccessat)libc_faccessat)(dirfd, passpath, mode, flags);
    }

    //int stat(const char *pathname, struct stat *statbuf){
    //    log_msg(INFO, "stat");
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    pass_getpath(pathname, passpath);
    //    log_msg(INFO, "stat %s", passpath);
    //    return ((funcptr_stat)libc_stat)(passpath, statbuf);
    //}

    int lstat(const char *pathname, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "lstat %s", passpath);
        return ((funcptr_lstat)libc_lstat)(passpath, statbuf);
    }

    int lstat64(const char *pathname, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "lstat64 %s", passpath);
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
        pass_getpath(path, passpath);
        log_msg(INFO, "fstatat %s", passpath);
        return ((funcptr_fstatat)libc_fstatat)(dirfd, passpath, statbuf, flags);
    }

    int statvfs(const char *path, struct statvfs *buf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "statvfs %s", path);
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        return ((funcptr_statvfs)libc_statvfs)(passpath, buf);
    }

    int __xstat(int ver, const char *path, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "xstat %s", passpath);
        return ((funcptr___xstat)libc___xstat)(ver, passpath, statbuf);
    }

    int _xstat(int ver, const char *path, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "xstat %s", passpath);
        return ((funcptr__xstat)libc__xstat)(ver, passpath, statbuf);
    }

    int __xstat64(int ver, const char *path, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "xstat64 %s", passpath);
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
        pass_getpath(path, passpath);
        log_msg(INFO, "fxstatat %s", passpath);
        return ((funcptr___fxstatat)libc___fxstatat)(ver, fd, passpath, statbuf, flag);
    }

    int __fxstat64(int ver, int fd, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fxstat64");
        return ((funcptr___fxstat64)libc___fxstat64)(ver, fd, statbuf);
    }

    int __fxstatat64(int ver, int fd, const char *path, struct stat64 *statbuf, int flag){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "fxstatat64 %s", passpath);
        return ((funcptr___fxstatat64)libc___fxstatat64)(ver, fd, passpath, statbuf, flag);
    }

    int __lxstat(int ver, const char *path, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "lxstat %s", passpath);
        return ((funcptr___lxstat)libc___lxstat)(ver, passpath, statbuf);
    }

    int __lxstat64(int ver, const char *path, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "lxstat64 %s", path);
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        return ((funcptr___lxstat64)libc___lxstat64)(ver, passpath, statbuf);
    }

    FILE* fopen(const char *path, const char *mode){
        if(!strcmp(path, get_config_file()))
        {
            // Disable functions because config parsing requires fopen
            initialize_functions();
            FILE * f = ((funcptr_fopen)libc_fopen)(path, mode);
            return f;
        }
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fopen %s %s", path, mode);
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        return ((funcptr_fopen)libc_fopen)(passpath, mode);
    }

    FILE* fopen64(const char *path, const char *mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "fopen %s %s", passpath, mode);
        return ((funcptr_fopen64)libc_fopen64)(passpath, mode);
    }

    FILE* freopen(const char *path, const char *mode, FILE *stream){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "freopen %s %s", passpath, mode);
        return ((funcptr_freopen)libc_freopen)(passpath, mode, stream);
    }

    FILE* freopen64(const char *path, const char *mode, FILE *stream){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "freopen64 %s %s", passpath, mode);
        return ((funcptr_freopen64)libc_freopen64)(passpath, mode, stream);
    }


    int truncate(const char *path, off_t offset){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        log_msg(INFO, "truncate %s", passpath);
        return ((funcptr_truncate)libc_truncate)(passpath, offset);
    }

    int ftruncate(int fd, off_t offset){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "ftruncate");
        return ((funcptr_ftruncate)libc_ftruncate)(fd, offset);
    }

    int chown(const char *pathname, uid_t owner, gid_t group){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "chown %s", passpath);
        return ((funcptr_chown)libc_chown)(passpath, owner, group);
    }

    int lchown(const char *pathname, uid_t owner, gid_t group){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "lchown %s", passpath);
        return ((funcptr_lchown)libc_lchown)(passpath, owner, group);
    }

    int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "fchownat %s", passpath);
        return ((funcptr_fchownat)libc_fchownat)(dirfd, passpath, owner, group, flags);
    }

    int chmod(const char *pathname, mode_t mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "chmod %s", passpath);
        return ((funcptr_chmod)libc_chmod)(passpath, mode);
    }

    int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        log_msg(INFO, "fchmodat %s", passpath);
        return ((funcptr_fchmodat)libc_fchmodat)(dirfd, passpath, mode, flags);
    }

    int setxattr(const char* path, const char *name, const void *value, size_t size, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "setxattr %s", path);
        char passpath[PATH_MAX];
        pass_getpath(path, passpath);
        return ((funcptr_setxattr)libattr_setxattr)(passpath, name, value, size, flags);
    }

    int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fsetxattr");
        return ((funcptr_fsetxattr)libattr_fsetxattr)(fd, name, value, size, flags);
    }

    const char* magic_file(magic_t cookie, const char *filename){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "magic_file");
        char passpath[PATH_MAX];
        pass_getpath(filename, passpath);
        return ((funcptr_magic_file)libmagic_magic_file)(cookie, passpath);
    }

    int euidaccess(const char *pathname, int mode){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "euidaccess");
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
        return ((funcptr_euidaccess)libc_euidaccess)(passpath, mode);
    }
    int eaccess(const char *pathname, int mode){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "eaccess");
        char passpath[PATH_MAX];
        pass_getpath(pathname, passpath);
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
        log_msg(INFO, "mkstemp");
        char passpath[PATH_MAX];
        pass_getpath(templ, passpath);
        int ret = ((funcptr_mkstemp)libc_mkstemp)(passpath);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkstemp64(char * templ){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "mkstemp64");
        char passpath[PATH_MAX];
        pass_getpath(templ, passpath);
        int ret = ((funcptr_mkstemp64)libc_mkstemp64)(passpath);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkostemp(char * templ, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "mkostemp");
        char passpath[PATH_MAX];
        pass_getpath(templ, passpath);
        int ret = ((funcptr_mkostemp)libc_mkostemp)(passpath, flags);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkostemp64(char * templ, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "mkostemp64");
        char passpath[PATH_MAX];
        pass_getpath(templ, passpath);
        templ = &passpath[0];
        int ret = ((funcptr_mkostemp64)libc_mkostemp64)(templ, flags);
        copy_last6(templ, passpath, 0);
        return ret;
    }
    int mkstemps(char * templ, int suffixlen){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "mkstemps");
        char passpath[PATH_MAX];
        pass_getpath(templ, passpath);
        int ret = ((funcptr_mkstemps)libc_mkstemps)(passpath, suffixlen);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkstemps64(char * templ, int suffixlen){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "mkstempts64");
        char passpath[PATH_MAX];
        pass_getpath(templ, passpath);
        int ret = ((funcptr_mkstemps64)libc_mkstemps64)(passpath, suffixlen);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkostemps(char * templ, int suffixlen, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "mkostemps");
        char passpath[PATH_MAX];
        pass_getpath(templ, passpath);
        int ret = ((funcptr_mkostemps)libc_mkostemps)(passpath, suffixlen, flags);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }
    int mkostemps64(char * templ, int suffixlen, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(templ, passpath);
        log_msg(INFO, "mkostemps64 %s", passpath);
        int ret = ((funcptr_mkostemps64)libc_mkostemps64)(passpath, suffixlen, flags);
        copy_last6(templ, passpath, suffixlen);
        return ret;
    }

    FILE * setmntent (const char *file, const char *mode){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(file, passpath);
        log_msg(INFO, "stmntent %s", passpath);
        return ((funcptr_setmntent)libc_setmntent)(passpath, mode);

    }

    char * bindtextdomain (const char *domainname, const char *dirname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(dirname, passpath);
        log_msg(INFO, "bindtextdomain %s", passpath);
        return ((funcptr_bindtextdomain)libc_bindtextdomain)(domainname, dirname);
    }

    int symlink (const char *oldname, const char *newname){
        initialize_passthrough_if_necessary();
        char old_passpath[PATH_MAX];
        char new_passpath[PATH_MAX];
        pass_getpath(oldname, old_passpath);
        pass_getpath(newname, new_passpath);
        log_msg(INFO, "symlink: old: %s new: %s", old_passpath, new_passpath);
        return ((funcptr_symlink)libc_symlink)(old_passpath, new_passpath);
    }

    ssize_t readlink (const char *filename, char *buffer, size_t size){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        pass_getpath(filename, passpath);
        log_msg(INFO, "readlink %s", passpath);
        return ((funcptr_readlink)libc_readlink)(passpath, buffer, size);
    }
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

