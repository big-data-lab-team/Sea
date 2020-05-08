/*
 * Copyright (c) 2014 by Matthias Noack, Zuse Institute Berlin
 *
 * Licensed under the BSD License, see LICENSE file for details.
 *
 */

#include "passthrough.h"

#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
//#include <unistd.h>

#define MAX_LOG 200
#define DEBUG_LVL 4
#define LOG_FOREGROUND 0 // currently doesn't really work when set to 1
#define MAX_MOUNTS 6

void* libc;
void* libc_open;
void* libc_openat;
void* libc_opendir;
void* libc_close;
void* libc___close;
void* libc_read;
void* libc_write;
void* libc_pread;
void* libc_pwrite;
void* libc_dup;
void* libc_dup2;
void* libc_lseek;
void* libc_rename;
void* libc_renameat;
void* libc_renameat2;
void* libc_remove;
void* libc_unlink;
void* libc_unlinkat;
void* libc_readdir;
void* libc_mkdir;
void* libc_access;
void* libc_faccessat;
void* libc_stat;
void* libc_fstat;
void* libc_fstatat;
void* libc_lstat;
void* libc_lstat64;
void* libc_statvfs;
void* libc___xstat;
void* libc___xstat64;
void* libc___fxstat;
void* libc___fxstatat;
void* libc___fxstat64;
void* libc___fxstatat64;
void* libc___lxstat;
void* libc___lxstat64;

void* libc_fopen;
void* libc_truncate;
void* libc_ftruncate;

void* libattr;
void* libattr_setxattr;
void* libattr_fsetxattr;

static const char* relmount = "./mount";
static char mount_dir[PATH_MAX];
static const char* source = "/dev/shm";
static const char* source_file = "./sources.txt";
static char source_mounts[MAX_MOUNTS][PATH_MAX];

// maybe use tmpnam?
static const char* log_fn = "passlogs.log";

enum LogLevel { DEBUG=4, INFO=3, WARNING=2, ERROR=1, NONE=0};

static const char* get_lvlname(int lvl){
    switch(lvl){
        case 1:
            return "ERROR";
        case 2:
            return "WARNING";
        case 3:
            return "INFO";
        default:
            return "DEBUG";
    }
}

static int log_msg(int lvl, const char* msg, ...){

    if (lvl > DEBUG_LVL)
        return 0;

    //get current time
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    //format input string
    char fmsg[MAX_LOG];
    va_list arglist;
    va_start( arglist, msg );
    vsprintf(fmsg, msg ,arglist);
    va_end( arglist );

    if (LOG_FOREGROUND)
        fprintf(stderr, "%s: %s: %s\n", strtok(asctime(timeinfo), "\n"), get_lvlname(lvl), fmsg);

    else {
        FILE* logs = log_fopen(log_fn, "a+");
        // write complete log string to file
        if (logs != NULL){
            fprintf(logs, "%s: %s: %s\n", strtok(asctime(timeinfo), "\n"), get_lvlname(lvl), fmsg);
        }
        fclose(logs);
    }

    return 0;
}

// Our "copy" of stdout, because the application might close stdout
// or reuse the first file descriptors for other purposes.
static FILE* fdout = 0;

FILE* xtreemfs_stdout() {
  return fdout;
}

// function that loads all the source mounts located in the sources file
static void init_sources(){

    log_msg(DEBUG, "initializing sources");
    FILE* fhierarchy = log_fopen(source_file, "r");
    log_msg(DEBUG, "log file opened");
    if (fhierarchy == NULL){
        log_msg(ERROR, "error opening source mount file\n");
        exit(1);
    }

    int i = 0;
    while (fgets(source_mounts[i], sizeof(source_mounts[i]), fhierarchy) != NULL){
        log_msg(DEBUG, "loading source %d", i);
        // Strip newline character from filename string if present
        int len = strlen(source_mounts[i]);
        if (len > 0 && source_mounts[i][len-1] == '\n')
            source_mounts[i][len - 1] = 0;
        log_msg(DEBUG, "sourcename %s.", source_mounts[i]);
        i++;
    }
    fclose(fhierarchy);


}

static int check_path_exists(const char *path, char fullpath[PATH_MAX]){
    int i = 0;
    int ret = 0;

    fprintf(stderr, "%s\n", path);
    // keep the best available temp path
    char path_holder[PATH_MAX];
    while(source_mounts[i][0] != '\0'){
        char tmp_path[PATH_MAX];
        strncpy(tmp_path, source_mounts[i], PATH_MAX);
        strncpy(path_holder, source_mounts[i], PATH_MAX);
        strncat(tmp_path, path, PATH_MAX);
        fprintf(stderr, "%s\n", tmp_path);

        struct stat sb;
        struct statvfs st_s;
        if( stat(tmp_path, &sb) == 0){
            // file exists
            ret = 1;
            strncpy(fullpath, tmp_path, PATH_MAX);
            break;
        }
        else if (statvfs(path_holder, &st_s) != 0 && st_s.f_bavail > 0 && st_s.f_favail > 0){
            strcpy(path_holder, tmp_path);
        }
        i++;

    }

    if (ret == 0){
        strcpy(fullpath, path_holder);
    }

    return ret;
}

static int get_path(const char* oldpath, char passpath[PATH_MAX]){
    char* match;
    char actualpath [PATH_MAX + 1];
    realpath(oldpath, actualpath);
    int len = strlen(mount_dir);
    strcpy(passpath, source);
    int match_found = 0;

    //log_msg(DEBUG, "actualpath: %s, mount_dir: %s", actualpath, mount_dir);

    if(mount_dir[0] != '\0' && (match = strstr(actualpath, mount_dir))){
        if (match == NULL)
            log_msg(DEBUG, "match null");
        log_msg(DEBUG, "match");
        *match = '\0';
        check_path_exists(match + len, passpath);
        //strcat(passpath, match + len);
        match_found = 1;
    }
    else{
        log_msg(DEBUG, "no match");
        strcpy(passpath, oldpath);
    }

    log_msg(INFO, "old fn %s ---> new fn %s", oldpath, passpath);
    return match_found;
}

static void initialize_passthrough() {
  xprintf("initialize_passthrough(): Setting up pass-through\n");
  libc = dlopen("libc.so.6", RTLD_LAZY); // TODO: link with correct libc, version vs. 32 bit vs. 64 bit
  libc_open = dlsym(libc, "open");
  libc_openat = dlsym(libc, "openat");
  libc_opendir = dlsym(libc, "opendir");
  libc_close = dlsym(libc, "close");
  libc___close = dlsym(libc, "__close");
  libc_read = dlsym(libc, "read");
  libc_write = dlsym(libc, "write");
  libc_pread = dlsym(libc, "pread");
  libc_pwrite = dlsym(libc, "pwrite");
  libc_dup = dlsym(libc, "dup");
  libc_dup2 = dlsym(libc, "dup2");
  libc_lseek = dlsym(libc, "lseek");

  libc_readdir = dlsym(libc, "readdir");
  libc_mkdir = dlsym(libc, "mkdir");
  libc_rename = dlsym(libc, "rename");
  libc_renameat = dlsym(libc, "renameat");
  libc_renameat2 = dlsym(libc, "renameat2");
  libc_remove = dlsym(libc, "remove");
  libc_unlink = dlsym(libc, "unlink");
  libc_unlinkat = dlsym(libc, "unlinkat");

  libc_access = dlsym(libc, "access");
  libc_faccessat = dlsym(libc, "faccessat");
  libc_stat = dlsym(libc, "stat");
  libc_lstat = dlsym(libc, "lstat");
  libc_lstat64 = dlsym(libc, "lstat64");
  libc_fstat = dlsym(libc, "fstat");
  libc_fstatat = dlsym(libc, "fstatat");
  libc_statvfs = dlsym(libc, "statvfs");
  libc___xstat = dlsym(libc, "__xstat");
  libc___xstat64 = dlsym(libc, "__xstat64");
  libc___fxstat = dlsym(libc, "__fxstat");
  libc___fxstatat = dlsym(libc, "__fxstatat");
  libc___fxstat64 = dlsym(libc, "__fxstat64");
  libc___fxstatat64 = dlsym(libc, "__fxstatat64");
  libc___lxstat = dlsym(libc, "__lxstat");
  libc___lxstat64 = dlsym(libc, "__lxstat64");

  libc_fopen = dlsym(libc, "fopen");
  libc_truncate = dlsym(libc, "truncate");
  libc_ftruncate = dlsym(libc, "ftruncate");

  libattr = dlopen("libattr.so.1", RTLD_LAZY);
  libattr_setxattr = dlsym(libattr, "setxattr");
  libattr_fsetxattr = dlsym(libattr, "setxattr");

  int stdout2 = ((funcptr_dup)libc_dup)(1);

  if (stdout2 != -1) {
    fdout = fdopen(stdout2, "a");
  }

  init_sources();

  //xprintf("initialize_passthrough(): New stdout %d\n", stdout2);
  if (realpath(relmount, mount_dir) == NULL)
      log_msg(ERROR, "Was not able to obtain absolute path of mount dir");
}

static pthread_once_t passthrough_initialized = PTHREAD_ONCE_INIT;

void initialize_passthrough_if_necessary() {
  pthread_once(&passthrough_initialized, initialize_passthrough);
}

//int open(const char* pathname, int flags, mode_t mode){
//    //fprintf(stdout, "opening file %s \n", pathname);
//    char passpath[PATH_MAX];
//    get_path(pathname, passpath);
//    initialize_passthrough_if_necessary();
//    return ((funcptr_open)libc_open)(pathname, flags, mode);
//}

FILE* log_fopen(const char *path, const char *mode){
    return ((funcptr_fopen)libc_fopen)(path, mode);
}
//int orig_stat(const char *pathname, struct stat *statbuf){
//    fprintf(stderr, "orig stat \n");
//    return ((funcptr_stat)libc_stat)(pathname, statbuf);
//}

extern "C" {

    int open(__const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "opening file %s", pathname);
        char passpath[PATH_MAX];
        get_path(pathname, passpath);

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
        get_path(pathname, passpath);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        return ((funcptr_open)libc_open)(passpath, flags, mode);
    }

    int __open64(__const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "open64 %s", pathname);
        char passpath[PATH_MAX];
        get_path(pathname, passpath);

        mode_t mode;
        if (flags & O_CREAT) {
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, mode_t);
            va_end(ap);
        }

        return ((funcptr_open)libc_open)(passpath, flags, mode);
    }

    int openat(int dirfd, const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        log_msg(INFO, "openat file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int openat64(int dirfd, const char* pathname, int flags, ...){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        log_msg(INFO, "openat64 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    int __openat64_2(int dirfd, const char* pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        log_msg(INFO, "openat64_2 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    extern int __openat_2(int dirfd, const char* pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        log_msg(INFO, "openat_2 file %s", passpath);
        return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
    }

    DIR* opendir(const char* pathname){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        log_msg(INFO, "opening directory %s", passpath);
        return ((funcptr_opendir)libc_opendir)(passpath);
    }

    //DIR* __opendir(const char* pathname){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    get_path(pathname, passpath);
    //    log_msg(INFO, "__opening directory %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    //DIR* opendir2(const char *name, int flags){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    get_path(name, passpath);
    //    log_msg(INFO, "opening directory 2 test %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    //DIR* __opendir2(const char *name, int flags){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    get_path(name, passpath);
    //    log_msg(INFO, "__opening directory 2 test %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    //DIR* opendirat (int dfd, const char *name){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    get_path(name, passpath);
    //    log_msg(INFO, "__opendirat %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}
    //DIR * __opendirat (int dfd, const char *name){
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    get_path(name, passpath);
    //    log_msg(INFO, "__opendirat %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    //static DIR* __opendir_common(int fd, const char *name, int flags)
    //{
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    get_path(name, passpath);
    //    log_msg(INFO, "opening directory 2 test %s", passpath);
    //    return ((funcptr_opendir)libc_opendir)(passpath);
    //}

    int scandir(const char *dirp, struct dirent ***namelist,
                  int (*filter)(const struct dirent *),
                  int (*compar)(const struct dirent **, const struct dirent **)){
        log_msg(INFO, "scandir test");
        return 0;
    }
    int scandir64(const char *dirp, struct dirent64 ***namelist,
                  int (*select)(const struct dirent64 *),
                  int (*cmp)(const struct dirent64 **, const struct dirent64 **)){
        log_msg(INFO, "scandir64 test");
        return 0;
    }

    int scandirat (int dfd, const char *dir, struct dirent ***namelist,
             int (*select) (const struct dirent *),
             int (*cmp) (const struct dirent **, const struct dirent **))
    {
        log_msg(INFO, "scandirat test");
        return 0;
    }

    int scandirat64 (int dfd, const char *dir, struct dirent64 ***namelist,
             int (*select) (const struct dirent64 *),
             int (*cmp) (const struct dirent64 **, const struct dirent64 **))
    {
        log_msg(INFO, "scandir64 test");
        return 0;
    }

    struct dirent *readdir(DIR *dirp){

        struct dirent *d;
        log_msg(INFO, "readdir");
        d = ((funcptr_readdir)libc_readdir)(dirp);

        if (d != NULL)
            log_msg(ERROR, "reading dir %s", d->d_name);
        return d;
    }


#undef creat
    int creat(__const char *name, mode_t mode) {
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
        get_path(pathname, passpath);
        log_msg(INFO, "mkdir path %s", passpath);
        return ((funcptr_mkdir)libc_mkdir)(passpath, mode);
    }

    int rename(const char *oldpath, const char *newpath){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        get_path(oldpath, oldpasspath);
        get_path(newpath, newpasspath);
        log_msg(INFO, "rename file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_rename)libc_rename)(oldpasspath, newpasspath);
    }

    int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        get_path(oldpath, oldpasspath);
        get_path(newpath, newpasspath);
        log_msg(INFO, "renameat file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_renameat)libc_renameat)(olddirfd, oldpasspath, newdirfd, newpasspath);
    }

    int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags){
        initialize_passthrough_if_necessary();
        char oldpasspath[PATH_MAX];
        char newpasspath[PATH_MAX];
        get_path(oldpath, oldpasspath);
        get_path(newpath, newpasspath);
        log_msg(INFO, "renameat2 file %s to %s", oldpasspath, newpasspath);
        return ((funcptr_renameat2)libc_renameat2)(olddirfd, oldpasspath, newdirfd, newpasspath, flags);
    }

    int remove(const char *pathname){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "remove file %s", pathname);
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        return ((funcptr_remove)libc_remove)(passpath);
    }

    int unlink(const char *pathname){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "unlink file %s", pathname);
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        return ((funcptr_unlink)libc_unlink)(passpath);
    }

    int unlinkat(int dirfd, const char *pathname, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        log_msg(INFO, "unlinkat file %s", passpath);
        return ((funcptr_unlinkat)libc_unlinkat)(dirfd, passpath, flags);
    }

    int access(const char *pathname, int mode){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "access path %s", pathname);
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        return ((funcptr_access)libc_access)(passpath, mode);
    }

    int faccessat(int dirfd, const char *pathname, int mode, int flags){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        log_msg(INFO, "faccessat path %s", passpath);
        return ((funcptr_faccessat)libc_faccessat)(dirfd, passpath, mode, flags);
    }

    //int stat(const char *pathname, struct stat *statbuf){
    //    log_msg(INFO, "stat");
    //    initialize_passthrough_if_necessary();
    //    char passpath[PATH_MAX];
    //    get_path(pathname, passpath);
    //    log_msg(INFO, "stat %s", passpath);
    //    return ((funcptr_stat)libc_stat)(passpath, statbuf);
    //}

    int lstat(const char *pathname, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
        log_msg(INFO, "lstat %s", passpath);
        return ((funcptr_lstat)libc_lstat)(passpath, statbuf);
    }

    int lstat64(const char *pathname, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(pathname, passpath);
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
        get_path(path, passpath);
        log_msg(INFO, "fstatat %s", passpath);
        return ((funcptr_fstatat)libc_fstatat)(dirfd, passpath, statbuf, flags);
    }

    int statvfs(const char *path, struct statvfs *buf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "statvfs %s", path);
        char passpath[PATH_MAX];
        get_path(path, passpath);
        return ((funcptr_statvfs)libc_statvfs)(passpath, buf);
    }

    int __xstat(int ver, const char *path, struct stat *statbuf){
        log_msg(INFO, "xstat");
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(path, passpath);
        log_msg(INFO, "xstat %s", passpath);
        return ((funcptr___xstat)libc___xstat)(ver, passpath, statbuf);
    }

    int __xstat64(int ver, const char *path, struct stat64 *statbuf){
        log_msg(INFO, "xstat64");
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(path, passpath);
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
        get_path(path, passpath);
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
        get_path(path, passpath);
        log_msg(INFO, "fxstatat64 %s", passpath);
        return ((funcptr___fxstatat64)libc___fxstatat64)(ver, fd, passpath, statbuf, flag);
    }

    int __lxstat(int ver, const char *path, struct stat *statbuf){
        initialize_passthrough_if_necessary();
        char passpath[PATH_MAX];
        get_path(path, passpath);
        log_msg(INFO, "lxstat %s", passpath);
        return ((funcptr___lxstat)libc___lxstat)(ver, passpath, statbuf);
    }

    int __lxstat64(int ver, const char *path, struct stat64 *statbuf){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "lxstat64 %s", path);
        char passpath[PATH_MAX];
        get_path(path, passpath);
        return ((funcptr___lxstat64)libc___lxstat64)(ver, passpath, statbuf);
    }

    FILE* fopen(const char *path, const char *mode){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fopen %s %s", path, mode);
        char passpath[PATH_MAX];
        get_path(path, passpath);
        return ((funcptr_fopen)libc_fopen)(passpath, mode);
    }


    int truncate(const char *path, off_t offset){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "truncate %s", path);
        char passpath[PATH_MAX];
        get_path(path, passpath);
        return ((funcptr_truncate)libc_truncate)(passpath, offset);
    }

    int ftruncate(int fd, off_t offset){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "ftruncate");
        return ((funcptr_ftruncate)libc_ftruncate)(fd, offset);
    }

    int setxattr(const char* path, const char *name, const void *value, size_t size, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "setxattr %s", path);
        char passpath[PATH_MAX];
        get_path(path, passpath);
        return ((funcptr_setxattr)libattr_setxattr)(passpath, name, value, size, flags);
    }

    int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags){
        initialize_passthrough_if_necessary();
        log_msg(INFO, "fsetxattr");
        return ((funcptr_fsetxattr)libattr_fsetxattr)(fd, name, value, size, flags);
    }
}
