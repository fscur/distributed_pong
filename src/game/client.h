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

typedef struct Client_State {
  void* memory;
  void* window;
  void* ui;
  void* input;
  void* rendering;
  void* network;
  bool running;
  f32 time;
  f32 dt;
  bool connected;
  char name[MAX_PLAYER_NAME_LENGTH];
  World* world;
  bool finished;
  i32 stage;
  Game_Client game_client;
} Client_State;

Client_State* client_create_state(Memory* memory);

void client_init(Client_State* state);
void client_load(Client_State* state);

void client_run(Client_State* state);

void client_input(Client_State* state);
void client_update(Client_State* state);
void client_render(const Client_State* state);
void client_unload(const Client_State* state);
void client_destroy(const Client_State* state);