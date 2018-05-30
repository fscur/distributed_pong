#define _GNU_SOURCE
#include <sys/mman.h>
#include "../game/server.h"
#include "time.c"
#include "network.c"

Memory
allocate_memory() {

#if _DEBUG
  void* base_addr = (void*)terabytes(1);
#else
  void* base_addr = 0;
#endif

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

  // note: (filipe)
  // if mprotect() fails we could not allocate all memory necessary and we
  // should quit

  assert(res == 0);
  return memory;
}

int
main(int argc, char** args) {
  Memory memory = allocate_memory();
  Server_State* state = server_create(&memory);

  server_init(state);
  server_load(state);

  Time_Spec last;
  Time_Spec current;
  double elapsed_seconds_per_frame;

  f32 monitor_hz = 60.0f;
  f32 desired_fps = 60.0f;
  f32 desired_seconds_per_frame = 1.0f / desired_fps;

  clock_gettime(CLOCK_MONOTONIC, &last);

  while (state->running) {

    server_run(state);

    clock_gettime(CLOCK_MONOTONIC_RAW, &current);
    elapsed_seconds_per_frame = to_seconds(&current) - to_seconds(&last);

    while (elapsed_seconds_per_frame <= desired_seconds_per_frame) {
      clock_gettime(CLOCK_MONOTONIC_RAW, &current);
      elapsed_seconds_per_frame = to_seconds(&current) - to_seconds(&last);

      // struct timespec sl;
      // sl.tv_sec = 0;
      // sl.tv_nsec = 1000000;
      // nanosleep(&sl, NULL);
    }

    last = current;

    state->dt = elapsed_seconds_per_frame;
    state->time += elapsed_seconds_per_frame;
  }

  // todo: (filipe)
  // should we not manually release our memory?
  // munmap(memory.permanent_storage_addr, memory.permanent_storage_size);
  server_unload(state);
  server_destroy(state);

  return 0;
}