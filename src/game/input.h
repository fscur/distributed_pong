#pragma once
#include "window.h"

typedef struct Input {
  Game_Window* window;
  void (*key_press)(void* state, i32 key);
} Input;

#define PLAYER1_UP 0
#define PLAYER1_DOWN 1
#define PLAYER2_UP 2
#define PLAYER2_DOWN 4

void input_init(Input* input);
void input_load(Input* input);
void input_update(Input* input);