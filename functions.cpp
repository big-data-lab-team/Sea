#include "functions.h"
#include "logger.h"

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
        log_msg(INFO, "scandir test");
        return 0;
    }
    int scandir64(const char *dirp, struct dirent64 ***namelist,
                  int (*select)(const struct dirent64 *),
                  int (*cmp)(const struct dirent64 **, const struct dirent64 **)){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "scandir64 test");
        return 0;
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
      log_msg(INFO, "creating file %s", name);
      return open(name, O_CREAT | O_WRONLY | O_TRUNC, mode);
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
        char passpath[PATH_MAX];
        pass_getpath(filename, passpath);
        return ((funcptr_magic_file)libmagic_magic_file)(cookie, passpath);

    }
}
