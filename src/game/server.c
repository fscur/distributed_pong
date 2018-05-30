#include "server.h"
#include "memory.c"
#include "bitmap.c"
#include "world.c"

// note: pixels/second
#define MOVE_SPEED 10.0f

#define AWAITING_STAGE 0
#define PLAYING_STAGE 1
#define RETRY_STAGE 2

internal void
key_press(Server_State* state, i32 key) {
  // World* world = (World*)state->world;

  // switch (key) {
  // case PLAYER1_UP:
  //   if (world->player_1.y < window->height - world->player_height) {
  //     world->player_1.velocity = MOVE_SPEED;
  //     world->player_1.y += world->player_1.velocity;
  //   }
  //   break;
  // case PLAYER1_DOWN:
  //   if (world->player_1.y > 0) {
  //     world->player_1.velocity = -MOVE_SPEED;
  //     world->player_1.y += world->player_1.velocity;
  //   }
  //   break;
  // case PLAYER2_UP:
  //   if (world->player_2.y < window->height - world->player_height) {
  //     world->player_2.velocity = MOVE_SPEED;
  //     world->player_2.y += world->player_2.velocity;
  //   }
  //   break;
  // case PLAYER2_DOWN:
  //   if (world->player_2.y > 0) {
  //     world->player_2.velocity = -MOVE_SPEED;
  //     world->player_2.y += world->player_2.velocity;
  //   }
  //   break;
  // }
}

Server_State*
server_create(Memory* memory) {
  assert(sizeof(Server_State) <= memory->permanent_size);

  Memory_Arena arena = {};
  arena_init(&arena, memory->permanent_addr, memory->permanent_size);

  Server_State* state = arena_push_struct(&arena, Server_State);
  state->world = arena_push_struct(&arena, World);
  state->network = arena_push_struct(&arena, Network_State);
  state->memory = memory;
  state->running = true;
  state->finished = false;
  state->dt = 0.0;
  state->stage = AWAITING_STAGE;

  World* world = (World*)state->world;
  world->width = 1280;
  world->height = 720;

  Network_State* network = state->network;
  network->world = state->world;
  network->connected_players = 0;
  sprintf(network->connection.server_ip, "127.0.0.1");
  network->connection.server_port = 60001;

  return state;
}

void
server_init(Server_State* state) {
  network_init_server(state->network);
  world_init(state->world);
}

void
server_load(Server_State* state) {}

void
server_run(Server_State* state) {
  switch (state->stage) {
  case AWAITING_STAGE: {
    if (network_accept_players(state->network)) {
      state->stage = PLAYING_STAGE;
      network_send_ready_message(state->network);
    }
    break;
  }
  case PLAYING_STAGE: {
    network_receive_input(state->network);
    server_update(state);
    network_broadcast(state->network);
    break;
  }
  }
}

void
server_update(Server_State* state) {

  World* world = (World*)state->world;
  world->time = state->time;
  world->dt = state->dt;

  if (world->player_1.points == MAX_POINTS ||
      world->player_2.points == MAX_POINTS) {
    state->stage = RETRY_STAGE;
    return;
  } else
    world_update(world);
}

void
server_unload(const Server_State* state) {}

void
server_destroy(const Server_State* state) {}