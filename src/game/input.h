#pragma once
#include "window.h"

typedef struct Input {
  Game_Window* window;
  void (*key_press)(void* state, i32 key);
} Input;

#define NO_INPUT 0
#define PLAYER_UP 1
#define PLAYER_DOWN 2

void input_init(Input* input);
void input_load(Input* input);
void input_update(Input* input);