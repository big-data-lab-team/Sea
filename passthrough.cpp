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

void* libc;
void* libc_open;
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


// Our "copy" of stdout, because the application might close stdout
// or reuse the first file descriptors for other purposes.
static FILE* fdout = 0;

FILE* xtreemfs_stdout() {
  return fdout;
}

static void initialize_passthrough() {
  xprintf("initialize_passthrough(): Setting up pass-through\n");
  libc = dlopen("libc.so.6", RTLD_LAZY); // TODO: link with correct libc, version vs. 32 bit vs. 64 bit
  libc_open = dlsym(libc, "open");
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
  xprintf("initialize_passthrough(): New stdout %d\n", stdout2);
}

static pthread_once_t passthrough_initialized = PTHREAD_ONCE_INIT;

void initialize_passthrough_if_necessary() {
  pthread_once(&passthrough_initialized, initialize_passthrough);
}

int open(const char* pathname, int flags, int mode){
    fprintf(stdout, "opening file %s \n", pathname);
    initialize_passthrough_if_necessary();
    return ((funcptr_open)libc_open)(pathname, flags, mode);
}

#undef creat
int creat(__const char *name, mode_t mode) {
  fprintf(stdout, "creating file\n");
  return open(name, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

int close(int fd){
    fprintf(stdout, "closing file\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_close)libc_close)(fd);
}

ssize_t read(int fd, void *buf, size_t count){
    initialize_passthrough_if_necessary();
    fprintf(stdout, "reading file of count %lu \n", count);
    return ((funcptr_read)libc_read)(fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count){
    fprintf(stdout, "writing file of count %lu\n", count);
    initialize_passthrough_if_necessary();
    return ((funcptr_write)libc_write)(fd, buf, count);
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset){
    fprintf(stdout, "preading file of count %lu\n", count);
    initialize_passthrough_if_necessary();
    return ((funcptr_pread)libc_pread)(fd, buf, count, offset);
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset){
    fprintf(stdout, "pwriting file of count %lu\n", count);
    initialize_passthrough_if_necessary();
    return ((funcptr_pwrite)libc_pwrite)(fd, buf, count, offset);
}

int dup(int oldfd){
    fprintf(stdout, "duplicating fd\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_dup)libc_dup)(oldfd);
}

int dup2(int oldfd, int newfd){
    fprintf(stdout, "duplicating 2 fd\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_dup2)libc_dup2)(oldfd, newfd);
}

off_t lseek(int fd, off_t offset, int whence){
    fprintf(stdout, "lseek\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_lseek)libc_lseek)(fd, offset, whence);
}

int stat(const char *pathname, struct stat *statbuf){
    fprintf(stdout, "stat %s\n", pathname);
    initialize_passthrough_if_necessary();
    return ((funcptr_stat)libc_stat)(pathname, statbuf);
}

int fstat(int fd, struct stat *statbuf){
    fprintf(stdout, "fstat\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_fstat)libc_fstat)(fd, statbuf);
}

int __xstat(int ver, const char *path, struct stat *statbuf){
    fprintf(stdout, "xstat %s\n", path);
    initialize_passthrough_if_necessary();
    return ((funcptr___xstat)libc___xstat)(ver, path, statbuf);
}

int __xstat64(int ver, const char *path, struct stat64 *statbuf){
    fprintf(stdout, "xstat64 %s\n", path);
    initialize_passthrough_if_necessary();
    return ((funcptr___xstat64)libc___xstat64)(ver, path, statbuf);
}

int __fxstat(int ver, int fd, struct stat *statbuf){
    fprintf(stdout, "fxstat \n");
    initialize_passthrough_if_necessary();
    return ((funcptr___fxstat)libc___fxstat)(ver, fd, statbuf);
}

int __fxstat64(int ver, int fd, struct stat64 *statbuf){
    fprintf(stdout, "fxstat64 \n");
    initialize_passthrough_if_necessary();
    return ((funcptr___fxstat64)libc___fxstat64)(ver, fd, statbuf);
}

int __lxstat(int ver, const char *path, struct stat *statbuf){
    fprintf(stdout, "lxstat %s\n", path);
    initialize_passthrough_if_necessary();
    return ((funcptr___lxstat)libc___lxstat)(ver, path, statbuf);
}

int __lxstat64(int ver, const char *path, struct stat64 *statbuf){
    fprintf(stdout, "lxstat64 %s\n", path);
    initialize_passthrough_if_necessary();
    return ((funcptr___lxstat64)libc___lxstat64)(ver, path, statbuf);
}

FILE* fopen(const char *path, const char *mode){
    fprintf(stdout, "fopen %s\n", path);
    initialize_passthrough_if_necessary();
    return ((funcptr_fopen)libc_fopen)(path, mode);
}

int truncate(const char *path, off_t offset){
    fprintf(stdout, "truncate %s\n", path);
    initialize_passthrough_if_necessary();
    return ((funcptr_truncate)libc_truncate)(path, offset);
}

int ftruncate(int fd, off_t offset){
    fprintf(stdout, "ftruncate\n");
    initialize_passthrough_if_necessary();
    return ((funcptr_ftruncate)libc_ftruncate)(fd, offset);
}

int setxattr(const char* path, const char *name, const void *value, size_t size, int flags){
    fprintf(stdout, "setxattr %s\n", path);
    initialize_passthrough_if_necessary();
    return ((funcptr_setxattr)libattr_setxattr)(path, name, value, size, flags);
}

int fsetxattr(int fd, const char *name, const void *value, size_t size, int flags){
    fprintf(stdout, "fsetxattr \n");
    initialize_passthrough_if_necessary();
    return ((funcptr_fsetxattr)libattr_fsetxattr)(fd, name, value, size, flags);
}
