#pragma once
#include <sys/stat.h>
#include <stdio.h>
#include "types.h"

#define FILE_CHUNK kilobytes(1)

size_t
io_get_file_size(const char* file_name) {
  FILE* file;
  size_t file_size;
  file = fopen(file_name, "r");
  fseek(file, 0L, SEEK_END);
  file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);
  fclose(file);
  return file_size;
}

void
io_read_text_file(const char* file_name, char* content) {

  FILE* file;
  size_t read_size;

  file = fopen(file_name, "r");

  if (file) {
    while ((read_size = fread(content, 1, FILE_CHUNK, file)) > 0) {
      content += read_size;
    }

    if (ferror(file)) {
      /* deal with error */
    }

    fclose(file);
  }
}

long
get_file_creation(char* file_name) {
  struct stat st;
  stat(file_name, &st);
  return (long)st.st_ctime;
}