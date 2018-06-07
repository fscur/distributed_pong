#define _GNU_SOURCE
#include <sys/mman.h>
#include "time.c"

#include "../game/client.h"

Memory
allocate_memory() {
  void* base_addr = 0;
  Memory memory = {};
  memory.permanent_size = megabytes(16);
  memory.transient_size = megabytes(16);

  u64 total_memory_size = memory.permanent_size + memory.transient_size;

  memory.permanent_addr = mmap(base_addr,
                               total_memory_size,
                               PROT_NONE,
                               MAP_PRIVATE | MAP_ANONYMOUS,
                               -1,
                               0);
  memory.transient_addr =
      (void*)(memory.permanent_addr + memory.permanent_size);

  u32 res = mprotect(
      memory.permanent_addr, total_memory_size, PROT_READ | PROT_WRITE);

  assert(res == 0);
  return memory;
}

int
main(int argc, char** args) {
  
  Memory memory = allocate_memory();
  Client_State* state = client_create_state(&memory);

  window_create(state, state->window);

  client_init(state);

  Time_Spec last;
  Time_Spec current;
  double elapsed_seconds_per_frame;

  f32 desired_fps = 60.0f;
  f32 desired_seconds_per_frame = 1.0f / desired_fps;

  clock_gettime(CLOCK_MONOTONIC, &last);

  while (state->running) {
    client_run(state);

    clock_gettime(CLOCK_MONOTONIC_RAW, &current);
    elapsed_seconds_per_frame = to_seconds(&current) - to_seconds(&last);

    while (elapsed_seconds_per_frame <= desired_seconds_per_frame) {
      clock_gettime(CLOCK_MONOTONIC_RAW, &current);
      elapsed_seconds_per_frame = to_seconds(&current) - to_seconds(&last);
    }

    last = current;

    state->dt = elapsed_seconds_per_frame;
    state->time += elapsed_seconds_per_frame;
  }

  client_destroy(state);

  return 0;
}