#pragma once

#include "types.h"

typedef struct Game_Window {
  void* handle;
  char* title;
  i32 width;
  i32 height;
  Color back_color;
  bool should_close;
} Game_Window;

typedef struct Game_State Game_State;

void window_create(Game_State* state, Game_Window* window);
void window_input(Game_Window* window);
void window_update(Game_Window* window);
void window_render(Game_Window* window);
void window_swapbuffers(Game_Window* window);
void window_destroy(Game_Window* window);