/*
 * Copyright (c) 2014 by Matthias Noack, Zuse Institute Berlin
 *
 * Licensed under the BSD License, see LICENSE file for details.
 *
 */

#include "logger.h"
#include "passthrough.h"

#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
//#include <unistd.h>

#define MAX_MOUNTS 6

void* libc;
void* libc_open;
void* libc___open;
void* libc_open64;
void* libc___open64;
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
void* libc_fopen64;
void* libc_truncate;
void* libc_ftruncate;

void* libc_chown;
void* libc_lchown;
void* libc_fchownat;

void* libattr;
void* libattr_setxattr;
void* libattr_fsetxattr;

void* libmagic;
void* libmagic_magic_file;

static const char* relmount = "./mount";
static char mount_dir[PATH_MAX];
static const char* source_file = "./sources.txt";
static char source_mounts[1][PATH_MAX];


// Our "copy" of stdout, because the application might close stdout
// or reuse the first file descriptors for other purposes.
static FILE* fdout = 0;

FILE* xtreemfs_stdout() {
  return fdout;
}

// function that loads all the source mounts located in the sources file
static void init_sources(){

    log_msg(DEBUG, "initializing sources");
    FILE* fhierarchy = ((funcptr_fopen)libc_fopen)(source_file, "r");
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
        log_msg(DEBUG, "sourcename: %s", source_mounts[i]);
        i++;
    }
    fclose(fhierarchy);


}

int pass_getpath(const char* oldpath, char passpath[PATH_MAX]){
    char* match;
    char actualpath [PATH_MAX + 1];
    realpath(oldpath, actualpath);
    int len = strlen(mount_dir);
    strcpy(passpath, source_mounts[0]);
    int match_found = 0;

    //log_msg(DEBUG, "actualpath: %s, mount_dir: %s", actualpath, mount_dir);

    if(mount_dir[0] != '\0' && (match = strstr(actualpath, mount_dir))){
        if (match == NULL)
            log_msg(DEBUG, "match null");
        log_msg(DEBUG, "match");
        *match = '\0';
        strcat(passpath, match + len);
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
  //xprintf("initialize_passthrough(): Setting up pass-through\n");
  libc = dlopen("libc.so.6", RTLD_LAZY); // TODO: link with correct libc, version vs. 32 bit vs. 64 bit
  libc_open = dlsym(libc, "open");
  libc___open = dlsym(libc, "__open");
  libc_open64 = dlsym(libc, "open64");
  libc___open64 = dlsym(libc, "__open64");
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
  libc_fopen64 = dlsym(libc, "fopen64");
  libc_truncate = dlsym(libc, "truncate");
  libc_ftruncate = dlsym(libc, "ftruncate");

  libc_chown = dlsym(libc, "chown");
  libc_lchown = dlsym(libc, "lchown");
  libc_fchownat = dlsym(libc, "fchownat");

  libattr = dlopen("libattr.so.1", RTLD_LAZY);
  libattr_setxattr = dlsym(libattr, "setxattr");
  libattr_fsetxattr = dlsym(libattr, "setxattr");

  //added libmagic
  libmagic = dlopen("libmagic.so.1", RTLD_LAZY);
  libmagic_magic_file = dlsym(libmagic, "magic_file");

  int stdout2 = ((funcptr_dup)libc_dup)(1);

  if (stdout2 != -1) {
    fdout = fdopen(stdout2, "a");
  }

  // Print the last error that occurred in dlsym, if any
  char * error = dlerror();
  if(error)
      xprintf("dlerror: %s\n",dlerror());

  init_sources();

  //xprintf("initialize_passthrough(): New stdout %d\n", stdout2);
  if (realpath(relmount, mount_dir) == NULL)
      log_msg(ERROR, "Was not able to obtain absolute path of mount dir");
}

static pthread_once_t passthrough_initialized = PTHREAD_ONCE_INIT;

void initialize_passthrough_if_necessary() {
  pthread_once(&passthrough_initialized, initialize_passthrough);
}

