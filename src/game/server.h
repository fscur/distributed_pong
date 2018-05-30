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

typedef struct Server_State {
  void* memory;
  void* network;
  bool running;
  f32 time;
  f32 dt;
  World* world;
  bool finished;
  i32 stage;
} Server_State;

Server_State* server_create(Memory* memory);

// note: (filipe)
// init occurs only once, before main loop
// destroy occurs only once, after main loop
// load/unload should be called every time the app is dynamically
// loaded/unloaded

void server_init(Server_State* state);
void server_load(Server_State* state);

void server_run(Server_State* state);

void server_unload(const Server_State* state);
void server_destroy(const Server_State* state);
