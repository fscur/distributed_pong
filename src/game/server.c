#include "server.h"
#include "memory.c"
#include "bitmap.c"
#include "world.c"
#include "time.h"

// note: pixels/second
#define MOVE_SPEED 10.0f

#define UNINITIALIZED_STAGE -1
#define AWAITING_STAGE 0
#define PLAYING_STAGE 1
#define RETRY_STAGE 2
#define GAME_OVER_STAGE 3

internal void
handle_input(Server_State* state, Player* player, i32 key) {
  World* world = (World*)state->world;

  switch (key) {
  case PLAYER_UP:
    if (player->y < world->height - world->player_height) {
      player->velocity = MOVE_SPEED;
      player->y += player->velocity;
    }
    break;
  case PLAYER_DOWN:
    if (player->y > 0) {
      player->velocity = -MOVE_SPEED;
      player->y += player->velocity;
    }
    break;
    // case PLAYER2_UP:
    //   if (world->player_2.y < world->height - world->player_height) {
    //     world->player_2.velocity = MOVE_SPEED;
    //     world->player_2.y += world->player_2.velocity;
    //   }
    //   break;
    // case PLAYER2_DOWN:
    //   if (world->player_2.y > 0) {
    //     world->player_2.velocity = -MOVE_SPEED;
    //     world->player_2.y += world->player_2.velocity;
    //   }
    break;
  }
}

internal void
change_stage(Server_State* state, i32 stage) {

  switch (stage) {
  case AWAITING_STAGE:
    printf("Awaiting players...\n");
    break;
  case PLAYING_STAGE:
    printf("Match started!\n");
    break;
  case RETRY_STAGE:
    printf("Match over!\n");
    break;
  }
  state->stage = stage;
}

internal void
on_goal(World* world, Player* player) {
  printf("%s scored!\n", player->name);
  printf("%s %d X %d %s\n",
         world->players[0].name,
         world->players[0].points,
         world->players[1].points,
         world->players[1].name);
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

  state->stage = UNINITIALIZED_STAGE;

  World* world = (World*)state->world;
  world->width = 1280;
  world->height = 720;
  world->on_goal = on_goal;

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
  change_stage(state, AWAITING_STAGE);
}

void
server_run(Server_State* state) {
  Network_State* network = state->network;
  World* world = state->world;

  switch (state->stage) {
  case AWAITING_STAGE: {
    if (network_accept_players(state->network)) {
      world_init(state->world);
      change_stage(state, PLAYING_STAGE);
      memcpy(world->players[0].name,
             network->clients[0].name,
             MAX_PLAYER_NAME_LENGTH);

      memcpy(world->players[1].name,
             network->clients[1].name,
             MAX_PLAYER_NAME_LENGTH);

      network_send_ready_message(state->network);
    }
    break;
  }
  case PLAYING_STAGE: {
    if (world->players[0].points == MAX_POINTS ||
        world->players[1].points == MAX_POINTS) {
      change_stage(state, RETRY_STAGE);
    } else {
      for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
        Game_Client client = network->clients[i];

        f64 start = time_now_seconds();
        i32 key = network_receive_input(state->network, &client);
        f64 end = time_now_seconds();

        if (end - start > 0.5) {
          network_send_game_over_message(
              network, &network->clients[(i + 1) % MAX_PLAYER_COUNT]);
          change_stage(state, RETRY_STAGE);
          return;
        }

        handle_input(state, &world->players[i], key);
      }

      world->time = state->time;
      world->dt = state->dt;
      world_update(world);
      network_broadcast(network);
    }
    break;
  }
  case RETRY_STAGE: {

    for (int i = 0; i < MAX_PLAYER_COUNT; ++i) {
      Game_Client* client = &network->clients[i];
      client->id = -1;
      __bzero(client->name, MAX_PLAYER_NAME_LENGTH);
      client->address.sin_addr.s_addr = 0;
      client->address.sin_port = 0;
      client->status = CLIENT_STATUS_DISCONNECTED;
    }

    network->connected_players = 0;
    network_clear_input_buffer(network->server_socket);
    change_stage(state, AWAITING_STAGE);
    break;
  }
  }
}

void
server_destroy(const Server_State* state) {
  // Network_State* network = state->network;
  // todo: close sockets
}