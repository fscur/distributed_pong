#pragma once
#include "types.h"
#include <stdlib.h>

typedef struct Memory {
  size_t permanent_size;
  u8* permanent_addr;
  size_t transient_size;
  u8* transient_addr;
} Memory;

typedef struct Memory_Arena {
  void* base_addr;
  void* current_addr;
  size_t size;
  size_t max_size;
} Memory_Arena;

#define arena_push_struct(arena_ptr, type)                                     \
  (type*)arena_push(arena_ptr, sizeof(type))

void arena_init(Memory_Arena* arena, void* base_addr, size_t max_size);
void* arena_push(Memory_Arena* arena, size_t size);