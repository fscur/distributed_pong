#pragma once
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include "../game/types.h"

void*
load_lib(char* lib_name) {
  void* lib_handle = dlopen(lib_name, RTLD_NOW);

  if (!lib_handle) {
    fprintf(stdout, "Couldn't open handle: %s\n", dlerror());
    exit(1);
  }
  return lib_handle;
}

void
unload_lib(void* lib_handle) {
  int result = dlclose(lib_handle);

  if (result != 0) {
    fprintf(stdout, "Couldn't close handle: %s\n", dlerror());
    exit(1);
  }
}

void*
reload_lib(void* lib_handle, char* lib_name, char* tmp_lib_name) {

  unload_lib(lib_handle);

  // note: (filipe)
  // make temp copy of shared object so compiler can write a new one
  int in_fd = open(lib_name, O_RDONLY);
  assert(in_fd >= 0);

  int out_fd = open(tmp_lib_name, O_WRONLY | O_CREAT);
  assert(out_fd >= 0);

  char buf[8192];

  while (1) {
    ssize_t result = read(in_fd, &buf[0], sizeof(buf));
    if (!result)
      break;
    assert(result > 0);
    assert(write(out_fd, &buf[0], result) == result);
  }

  // note: (filipe)
  // give permission to read and write to this file
  chmod(tmp_lib_name, S_IRUSR | S_IWUSR);
  return load_lib(tmp_lib_name);
}

void*
fn(void* lib_handle, char* fn_name) {
  void* function = dlsym(lib_handle, fn_name);

  if (function == NULL) {
    fprintf(stderr, "Couldn't find function: %s\n", fn_name);
    exit(1);
  }
  return function;
}