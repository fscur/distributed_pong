#define _GNU_SOURCE
#include <sys/mman.h>
#include <errno.h>
#include "lib_loader.c"
#include "../game/game.h"
#include "time.c"

#define GAME_LIB_NAME "lib_game.so"
#define TMP_GAME_LIB_NAME "tmp_lib_game.so"

void
load_game_code(Game_Code* code) {
  code->create = (Game_State * (*)(Memory*))fn(code->lib_handle, "game_create");
  code->init = (void (*)(Game_State*))fn(code->lib_handle, "game_init");
  code->load = (void (*)(Game_State*))fn(code->lib_handle, "game_load");
  code->input = (void (*)(Game_State*))fn(code->lib_handle, "game_input");
  code->update = (void (*)(Game_State*))fn(code->lib_handle, "game_update");
  code->render = (void (*)(Game_State*))fn(code->lib_handle, "game_render");
  code->unload = (void (*)(Game_State*))fn(code->lib_handle, "game_unload");
  code->destroy = (void (*)(Game_State*))fn(code->lib_handle, "game_destroy");
}

Game_Code
load_game() {
  Game_Code code = {};
  code.lib_handle = load_lib(GAME_LIB_NAME);
  load_game_code(&code);
  return code;
}

void
reload_game(Game_Code* code) {
  code->lib_handle =
      reload_lib(code->lib_handle, GAME_LIB_NAME, TMP_GAME_LIB_NAME);
  load_game_code(code);
}

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
  Game_Code game = load_game();
  Memory memory = allocate_memory();
  Game_State* state = game.create(&memory);

  // note: (filipe)
  // window must be created in the main or else it would be lost when reloading
  window_create(state, state->window);

  game.init(state);
  game.load(state);

  i64 last_modified = 0;
  bool should_reload = false;

  Time_Spec last;
  Time_Spec current;
  double elapsed_seconds_per_frame;

  f32 monitor_hz = 60.0f;
  f32 desired_fps = 60.0f;
  f32 desired_seconds_per_frame = 1.0f / desired_fps;

  clock_gettime(CLOCK_MONOTONIC, &last);

  while (state->running) {
    // #if _DEBUG
    //     if (should_reload) {

    //       game.unload(state);
    //       spin(500);
    //       reload_game(&game);
    //       game.load(state);

    //       should_reload = false;
    //     }

    //     if (get_file_creation(GAME_LIB_NAME) > last_modified) {
    //       last_modified = get_file_creation(GAME_LIB_NAME);
    //       should_reload = true;
    //     }
    // #endif

    game.render(state);
    game.input(state);
    game.update(state);

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

  // todo: (filipe)
  // should we not manually release our memory?
  // munmap(memory.permanent_storage_addr, memory.permanent_storage_size);
  game.unload(state);
  game.destroy(state);

  return 0;
}