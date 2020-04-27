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
#include <unistd.h>

#define MAX_LOG 200
#define DEBUG_LVL 4
#define LOG_FOREGROUND 0 // currently doesn't really work when set to 1

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
void* libc_remove;
void* libc_unlink;
void* libc_unlinkat;
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

static void get_path(const char* oldpath, char passpath[PATH_MAX]){
    char* match;
    char actualpath [PATH_MAX + 1];
    realpath(oldpath, actualpath);
    int len = strlen(mount_dir);
    strcpy(passpath, source);

    log_msg(DEBUG, "actualpath: %s, mount_dir: %s", actualpath, mount_dir);

    if(mount_dir[0] != '\0' && (match = strstr(actualpath, mount_dir))){
        if (match == NULL)
            log_msg(DEBUG, "match null");
        log_msg(DEBUG, "match");
        *match = '\0';
        strcat(passpath, match + len);
    }
    else{
        strcpy(passpath, oldpath);
    }

    log_msg(INFO, "old fn %s ---> new fn %s", oldpath, passpath);
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

  libc_mkdir = dlsym(libc, "mkdir");
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

  xprintf("initialize_passthrough(): New stdout %d\n", stdout2);
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

int openat(int dirfd, const char* pathname, int flags){
    initialize_passthrough_if_necessary();
    log_msg(INFO, "opening at file %s", pathname);
    char passpath[PATH_MAX];
    get_path(pathname, passpath);
    return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
}

DIR* opendir(const char* pathname){
    initialize_passthrough_if_necessary();
    log_msg(INFO, "opening directory %s", pathname);
    char passpath[PATH_MAX];
    get_path(pathname, passpath);
    return ((funcptr_opendir)libc_opendir)(passpath);
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
    log_msg(INFO, "mkdir path %s", pathname);
    char passpath[PATH_MAX];
    get_path(pathname, passpath);
    return ((funcptr_mkdir)libc_mkdir)(passpath, mode);
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
    log_msg(INFO, "unlinkat file %s", pathname);
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

int stat(const char *pathname, struct stat *statbuf){
    initialize_passthrough_if_necessary();
    log_msg(INFO, "stat %s", pathname);
    char passpath[PATH_MAX];
    get_path(pathname, passpath);
    return ((funcptr_stat)libc_stat)(passpath, statbuf);
}

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
    initialize_passthrough_if_necessary();
    log_msg(INFO, "xstat %s", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
    return ((funcptr___xstat)libc___xstat)(ver, passpath, statbuf);
}

int __xstat64(int ver, const char *path, struct stat64 *statbuf){
    initialize_passthrough_if_necessary();
    log_msg(INFO, "xstat64 %s", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
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

FILE* log_fopen(const char *path, const char *mode){
    return ((funcptr_fopen)libc_fopen)(path, mode);
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
