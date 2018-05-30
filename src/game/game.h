#pragma once

#include "types.h"
#include "bitmap.h"
#include "math.h"
#include "window.h"
#include "input.h"
#include "ui.h"
#include "memory.h"
#include "world.h"
#include "rendering.h"
#include "network.h"

typedef struct Game_State {
  void* memory;
  void* window;
  void* ui;
  void* input;
  void* rendering;
  void* network;
  bool running;
  f32 time;
  f32 dt;
  i64 frame_count;
  World* world;
  bool finished;
} Game_State;

typedef struct Game_Code {
  void* lib_handle;
  Game_State* (*create)(Memory*);
  void (*init)(Game_State*);
  void (*load)(Game_State*);
  void (*input)(Game_State*);
  void (*input_client)(Game_State*);
  void (*update)(Game_State*);
  void (*update_client)(Game_State*);
  void (*render)(Game_State*);
  void (*connect_to_server)(Game_State*);
  void (*receive_data)(Game_State*);
  void (*wait_players)(Game_State*);
  void (*broadcast)(Game_State*);
  void (*unload)(Game_State*);
  void (*destroy)(Game_State*);
} Game_Code;

// note: (filipe)
// create is where our memory is allocated
Game_State* game_create(Memory* memory);

// note: (filipe)
// init occurs only once, before main loop
// destroy occurs only once, after main loop
// load/unload should be called every time the app is dynamically
// loaded/unloaded

void game_init(Game_State* state);
void game_load(Game_State* state);

void game_input(Game_State* state);
void game_input_client(Game_State* state);
void game_update(Game_State* state);
void game_update_client(Game_State* state);
void game_render(const Game_State* state);

void game_connect_to_server(const Game_State* state);
void game_receive_data(const Game_State* state);
void game_wait_players(const Game_State* state);
void game_broadcast(const Game_State* state);

void game_unload(const Game_State* state);
void game_destroy(const Game_State* state);
