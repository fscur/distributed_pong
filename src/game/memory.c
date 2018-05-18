#include "memory.h"

void
arena_init(Memory_Arena* arena, void* base_addr, size_t max_size) {
  assert(max_size > 0);
  assert(base_addr != NULL);
  arena->base_addr = base_addr;
  arena->current_addr = base_addr;
  arena->size = 0;
  arena->max_size = max_size;
}

void*
arena_push(Memory_Arena* arena, size_t size) {
  assert(arena->size + size <= arena->max_size);
  void* ret = arena->current_addr;
  arena->size += size;
  arena->current_addr += size;
  return ret;
}