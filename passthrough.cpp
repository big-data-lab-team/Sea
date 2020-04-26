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
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

void* libc;
void* libc_open;
void* libc_openat;
void* libc_close;
void* libc___close;
void* libc_read;
void* libc_write;
void* libc_pread;
void* libc_pwrite;
void* libc_dup;
void* libc_dup2;
void* libc_lseek;
void* libc_stat;
void* libc_fstat;
void* libc___xstat;
void* libc___xstat64;
void* libc___fxstat;
void* libc___fxstat64;
void* libc___lxstat;
void* libc___lxstat64;

void* libc_fopen;
void* libc_truncate;
void* libc_ftruncate;

void* libattr;
void* libattr_setxattr;
void* libattr_fsetxattr;

static char mount_dir[PATH_MAX];
static const char* source = "/dev/shm";


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
    fprintf(stderr, "actualpath %s %s\n", actualpath, mount_dir);

    if(mount_dir[0] != '\0' && (match = strstr(actualpath, mount_dir))){
        if (match == NULL)
            fprintf(stderr, "match null\n");
        fprintf(stderr, "match\n");
        *match = '\0';
        strcat(passpath, match + len);
    }
    else{
        strcpy(passpath, oldpath);
    }

    fprintf(stdout, "old fn %s ---> new fn %s\n", oldpath, passpath);
}

static void initialize_passthrough() {
  xprintf("initialize_passthrough(): Setting up pass-through\n");
  libc = dlopen("libc.so.6", RTLD_LAZY); // TODO: link with correct libc, version vs. 32 bit vs. 64 bit
  libc_open = dlsym(libc, "open");
  libc_openat = dlsym(libc, "openat");
  libc_close = dlsym(libc, "close");
  libc___close = dlsym(libc, "__close");
  libc_read = dlsym(libc, "read");
  libc_write = dlsym(libc, "write");
  libc_pread = dlsym(libc, "pread");
  libc_pwrite = dlsym(libc, "pwrite");
  libc_dup = dlsym(libc, "dup");
  libc_dup2 = dlsym(libc, "dup2");
  libc_lseek = dlsym(libc, "lseek");

  libc_stat = dlsym(libc, "stat");
  libc_fstat = dlsym(libc, "fstat");
  libc___xstat = dlsym(libc, "__xstat");
  libc___xstat64 = dlsym(libc, "__xstat64");
  libc___fxstat = dlsym(libc, "__fxstat");
  libc___fxstat64 = dlsym(libc, "__fxstat64");
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

  if (getcwd(mount_dir, sizeof(mount_dir)) == NULL)
      fprintf(stdout, "ERROR\n");

  xprintf("initialize_passthrough(): New stdout %d\n", stdout2);
}

static pthread_once_t passthrough_initialized = PTHREAD_ONCE_INIT;

void initialize_passthrough_if_necessary() {
  pthread_once(&passthrough_initialized, initialize_passthrough);
}

//int open(const char* pathname, int flags, mode_t mode){
//    fprintf(stdout, "opening file %s \n", pathname);
//    char passpath[PATH_MAX];
//    get_path(pathname, passpath);
//    initialize_passthrough_if_necessary();
//    return ((funcptr_open)libc_open)(pathname, flags, mode);
//}

int open(__const char* pathname, int flags, ...){
    fprintf(stderr, "opening file %s \n", pathname);
    char passpath[PATH_MAX];
    get_path(pathname, passpath);

    mode_t mode;
    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    }

    initialize_passthrough_if_necessary();
    return ((funcptr_open)libc_open)(passpath, flags, mode);
}

int openat(int dirfd, const char* pathname, int flags){
    fprintf(stderr, "opening at file %s \n", pathname);
    char passpath[PATH_MAX];
    get_path(pathname, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr_openat)libc_openat)(dirfd, passpath, flags);
}


#undef creat
int creat(__const char *name, mode_t mode) {
  fprintf(stderr, "creating file %s\n", name);
  return open(name, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

int close(int fd){
    fprintf(stderr, "closing file\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_close)libc_close)(fd);
}

ssize_t read(int fd, void *buf, size_t count){
    initialize_passthrough_if_necessary();
    fprintf(stderr, "reading file of count %lu from fd %d\n", count, fd);

    size_t bytes = ((funcptr_read)libc_read)(fd, buf, count);
    //fprintf(stdout, "buffer contents: %s\n", buf);
    return bytes;
}

ssize_t write(int fd, const void *buf, size_t count){
    fprintf(stderr, "writing file of count %lu from fd %d\n", count, fd);
    initialize_passthrough_if_necessary();
    return ((funcptr_write)libc_write)(fd, buf, count);
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset){
    fprintf(stderr, "preading file of count %lu\n", count);
    initialize_passthrough_if_necessary();
    return ((funcptr_pread)libc_pread)(fd, buf, count, offset);
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset){
    fprintf(stderr, "pwriting file of count %lu\n", count);
    initialize_passthrough_if_necessary();
    return ((funcptr_pwrite)libc_pwrite)(fd, buf, count, offset);
}

int dup(int oldfd){
    fprintf(stderr, "duplicating fd\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_dup)libc_dup)(oldfd);
}

int dup2(int oldfd, int newfd){
    fprintf(stderr, "duplicating 2 fd\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_dup2)libc_dup2)(oldfd, newfd);
}

off_t lseek(int fd, off_t offset, int whence){
    fprintf(stderr, "lseek\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_lseek)libc_lseek)(fd, offset, whence);
}

int stat(const char *pathname, struct stat *statbuf){
    fprintf(stderr, "stat %s\n", pathname);
    char passpath[PATH_MAX];
    get_path(pathname, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr_stat)libc_stat)(passpath, statbuf);
}

int fstat(int fd, struct stat *statbuf){
    fprintf(stderr, "fstat\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_fstat)libc_fstat)(fd, statbuf);
}

int __xstat(int ver, const char *path, struct stat *statbuf){
    fprintf(stderr, "xstat %s\n", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr___xstat)libc___xstat)(ver, passpath, statbuf);
}

int __xstat64(int ver, const char *path, struct stat64 *statbuf){
    fprintf(stderr, "xstat64 %s\n", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr___xstat64)libc___xstat64)(ver, passpath, statbuf);
}

int __fxstat(int ver, int fd, struct stat *statbuf){
    fprintf(stderr, "fxstat \n");
    initialize_passthrough_if_necessary();
    return ((funcptr___fxstat)libc___fxstat)(ver, fd, statbuf);
}

int __fxstat64(int ver, int fd, struct stat64 *statbuf){
    fprintf(stderr, "fxstat64 \n");
    initialize_passthrough_if_necessary();
    return ((funcptr___fxstat64)libc___fxstat64)(ver, fd, statbuf);
}

int __lxstat(int ver, const char *path, struct stat *statbuf){
    fprintf(stderr, "lxstat %s\n", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr___lxstat)libc___lxstat)(ver, passpath, statbuf);
}

int __lxstat64(int ver, const char *path, struct stat64 *statbuf){
    fprintf(stderr, "lxstat64 %s\n", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr___lxstat64)libc___lxstat64)(ver, passpath, statbuf);
}

FILE* fopen(const char *path, const char *mode){
    fprintf(stderr, "fopen %s\n", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr_fopen)libc_fopen)(passpath, mode);
}

int truncate(const char *path, off_t offset){
    fprintf(stderr, "truncate %s\n", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr_truncate)libc_truncate)(passpath, offset);
}

int ftruncate(int fd, off_t offset){
    fprintf(stderr, "ftruncate\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_ftruncate)libc_ftruncate)(fd, offset);
}

int setxattr(const char* path, const char *name, const void *value, size_t size, int flags){
    fprintf(stderr, "setxattr %s\n", path);
    char passpath[PATH_MAX];
    get_path(path, passpath);
    initialize_passthrough_if_necessary();
    return ((funcptr_setxattr)libattr_setxattr)(passpath, name, value, size, flags);
}

int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags){
    fprintf(stderr, "fsetxattr \n");
    initialize_passthrough_if_necessary();
    return ((funcptr_fsetxattr)libattr_fsetxattr)(fd, name, value, size, flags);
}
