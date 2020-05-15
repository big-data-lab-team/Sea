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
#include <errno.h>

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
void* libc_chdir;
void* libc_access;
void* libc_faccessat;
void* libc_stat;
void* libc_fstat;
void* libc_fstatat;
void* libc_lstat;
void* libc_lstat64;
void* libc_statvfs;
void* libc___xstat;
void* libc__xstat;
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

void* libattr;
void* libattr_setxattr;
void* libattr_fsetxattr;

void* libmagic;
void* libmagic_magic_file;

static char relmount[PATH_MAX];
static char mount_dir[PATH_MAX];
static char source_file[PATH_MAX];
static char source_mounts[1][PATH_MAX];


static char * get_sea_home()
{
    char* sea_home;
    sea_home = getenv("PWD");
    return sea_home;
}

static void init_paths()
{
    char* sea_home = get_sea_home();
    strcat(relmount, sea_home);
    strcat(relmount, "/mount");

    strcat(source_file, sea_home);
    strcat(source_file, "/sources.txt");
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

// Taken from https://stackoverflow.com/questions/11034002/how-to-get-absolute-path-of-file-or-directory-that-does-not-exist
////////////////////////////////////////////////////////////////////////////////
// Return the input path in a canonical form. This is achieved by expanding all
// symbolic links, resolving references to "." and "..", and removing duplicate
// "/" characters.
//
// If the file exists, its path is canonicalized and returned. If the file,
// or parts of the containing directory, do not exist, path components are
// removed from the end until an existing path is found. The remainder of the
// path is then appended to the canonical form of the existing path,
// and returned. Consequently, the returned path may not exist. The portion
// of the path which exists, however, is represented in canonical form.
//
// If successful, this function returns a C-string, which needs to be freed by
// the caller using free().
//
// ARGUMENTS:
//   file_path
//   File path, whose canonical form to return.
//
// RETURNS:
//   On success, returns the canonical path to the file, which needs to be freed
//   by the caller.
//
//   On failure, returns NULL.
////////////////////////////////////////////////////////////////////////////////
char *make_file_name_canonical(char const *file_path)
{
  char *canonical_file_path  = NULL;
  unsigned int file_path_len = strlen(file_path);

  if (file_path_len > 0)
  {
    canonical_file_path = realpath(file_path, NULL);
    if (canonical_file_path == NULL && errno == ENOENT)
    {
      // The file was not found. Back up to a segment which exists,
      // and append the remainder of the path to it.
      char *file_path_copy = NULL;
      if (file_path[0] == '/'                ||
          (strncmp(file_path, "./", 2) == 0) ||
          (strncmp(file_path, "../", 3) == 0))
      {
        // Absolute path, or path starts with "./" or "../"
        file_path_copy = strdup(file_path);
      }
      else
      {
        // Relative path
        file_path_copy = (char*)malloc(strlen(file_path) + 3);
        strcpy(file_path_copy, "./");
        strcat(file_path_copy, file_path);
      }

      // Remove path components from the end, until an existing path is found
      for (int char_idx = strlen(file_path_copy) - 1;
           char_idx >= 0 && canonical_file_path == NULL;
           --char_idx)
      {
        if (file_path_copy[char_idx] == '/')
        {
          // Remove the slash character
          file_path_copy[char_idx] = '\0';

          canonical_file_path = realpath(file_path_copy, NULL);
          if (canonical_file_path != NULL)
          {
            // An existing path was found. Append the remainder of the path
            // to a canonical form of the existing path.
            char *combined_file_path = (char*)malloc(strlen(canonical_file_path) + strlen(file_path_copy + char_idx + 1) + 2);
            strcpy(combined_file_path, canonical_file_path);
            strcat(combined_file_path, "/");
            strcat(combined_file_path, file_path_copy + char_idx + 1);
            free(canonical_file_path);
            canonical_file_path = combined_file_path;
          }
          else
          {
            // The path segment does not exist. Replace the slash character
            // and keep trying by removing the previous path component.
            file_path_copy[char_idx] = '/';
          }
        }
      }

      free(file_path_copy);
    }
  }

  return canonical_file_path;
}
int pass_getpath(const char* oldpath, char passpath[PATH_MAX]){
    char* match;
    char* actualpath;
    actualpath = make_file_name_canonical(oldpath);
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
  init_paths();

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
  libc_chdir = dlsym(libc, "chdir");
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
  libc__xstat = dlsym(libc, "_xstat");
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

