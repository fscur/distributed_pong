#include "client.h"
#include "memory.c"
#include "bitmap.c"

#include <strings.h>
#include "time.h"
#include "world.c"

#define START_STAGE 0
#define SEARCHING_SERVER_STAGE 1
#define AWAITING_STAGE 2
#define PLAYING_STAGE 3
#define RETRY_STAGE 4
#define GAME_OVER_STAGE 5

void
stage0_play(Client_State* state,
            char name[MAX_PLAYER_NAME_LENGTH],
            char server_ip[SERVER_IP_LENGTH]) {

  memcpy(state->name, name, MAX_PLAYER_NAME_LENGTH);

  Network_State* network = (Network_State*)state->network;
  memcpy(network->connection.server_ip, server_ip, SERVER_IP_LENGTH);

  state->stage = SEARCHING_SERVER_STAGE;
}

void
client_on_close(Client_State* state) {
  state->running = false;
}

internal void
client_key_press(Client_State* state, i32 key) {
  network_send_input(state->network, key);
}

Client_State*
client_create_state(Memory* memory) {
  assert(sizeof(Client_State) <= memory->permanent_size);

  Memory_Arena arena = {};
  arena_init(&arena, memory->permanent_addr, memory->permanent_size);

  Client_State* state = arena_push_struct(&arena, Client_State);
  state->window = arena_push_struct(&arena, Game_Window);
  state->input = arena_push_struct(&arena, Input);
  state->ui = arena_push_struct(&arena, Ui);
  state->rendering = arena_push_struct(&arena, Render_State);
  state->network = arena_push_struct(&arena, Network_State);
  state->world = arena_push_struct(&arena, World);
  state->world_rendering_data = arena_push_struct(&arena, World_Rendering_Data);
  state->memory = memory;
  state->running = true;
  state->match_result = false;
  state->dt = 0.0;
  state->stage = START_STAGE;

  Color color = (Color){0.07f, 0.07f, 0.09f, 1.00f};

  Game_Window* window = state->window;
  window->title = "pong";
  window->width = 1280;
  window->height = 720;
  window->back_color = color;
  window->on_close = client_on_close;

  Input* input = state->input;
  input->window = state->window;
  input->key_press = client_key_press;

  Ui* ui = state->ui;
  ui->state = state;
  ui->window = state->window;
  ui->world = state->world;
  ui->play = stage0_play;
  ui->close = client_on_close;

  Network_State* network = state->network;
  network->world = state->world;
  sprintf(network->connection.server_ip, "127.0.0.1");
  network->connection.server_port = 60001;

  return state;
}

void
client_init(Client_State* state) {
  ui_init(state->ui);

  Game_Window* window = (Game_Window*)state->window;

  World* world = (World*)state->world;
  world->height = window->height;
  world->width = window->width;

  Render_State* render = (Render_State*)state->rendering;
  render->uniform_buffer_data = state->world_rendering_data;
  render->uniform_buffer_data_size = sizeof(World_Rendering_Data);
  rendering_init(render);

  Network_State* network = (Network_State*)state->network;
  network_init_client(network);

  ui_load(state->ui);
}

void
client_update(Client_State* state) {
  World* world = (World*)state->world;
  Network_State* network = state->network;

  world->players[0].points = network->current_packet.players_points[0];
  world->players[1].points = network->current_packet.players_points[1];

  if (world->players[0].points == MAX_POINTS ||
      world->players[1].points == MAX_POINTS) {
    state->stage = RETRY_STAGE;
    state->match_result =
        network->current_packet.players_points[state->id] == MAX_POINTS;
    return;
  }

  Game_Window* window = (Game_Window*)state->window;

  World_Rendering_Data* world_rendering_data =
      (World_Rendering_Data*)state->world_rendering_data;

  Ui* ui = (Ui*)state->ui;

  memcpy(world->players[0].name,
         network->current_packet.players_names[0],
         MAX_PLAYER_NAME_LENGTH);

  memcpy(world->players[1].name,
         network->current_packet.players_names[1],
         MAX_PLAYER_NAME_LENGTH);

  world_rendering_data->width = (float)window->width;
  world_rendering_data->height = (float)window->height;
  world_rendering_data->player_width = (float)world->player_width;
  world_rendering_data->player_height = (float)world->player_height;
  world_rendering_data->time = state->time;
  world_rendering_data->dt = state->dt;

  world_rendering_data->player_1_y =
      network->current_packet.players_positions[0];
  world_rendering_data->player_2_y =
      network->current_packet.players_positions[1];

  world_rendering_data->ball_position = network->current_packet.ball_pos;
  world_rendering_data->ball_radius = network->current_packet.ball_radius;

  ui->time = state->time;
  ui->dt = state->dt;

  state->running &= !window->should_close;
}

void
client_run(Client_State* state) {
  Game_Window* window = (Game_Window*)state->window;
  Network_State* network = (Network_State*)state->network;

  switch (state->stage) {
  case START_STAGE: {
    window_input(state->window);
    window_update(state->window);
    window_render(state->window);
    ui_render_start_screen(state->ui);
    window_swapbuffers(state->window);
    state->running &= !window->should_close;
    break;
  }
  case SEARCHING_SERVER_STAGE: {
    window_input(state->window);
    window_update(state->window);

    window_render(state->window);
    ui_render_searching_server(state->ui);
    window_swapbuffers(state->window);

    if (network_connect_to_server(state->network, state->name, &state->id)) {
      state->stage = AWAITING_STAGE;
    }

    state->running &= !window->should_close;
    break;
  }
  case AWAITING_STAGE: {
    bzero(&network->current_packet, sizeof(Game_Packet));

    world_init(state->world);
    window_input(state->window);
    window_update(state->window);

    window_render(state->window);
    ui_render_awaiting_challenger(state->ui);
    window_swapbuffers(state->window);

    if (network_receive_ready_message(state->network))
      state->stage = PLAYING_STAGE;

    state->running &= !window->should_close;

    break;
  }
  case PLAYING_STAGE: {
    window_input(state->window);
    input_update(state->input);

    f64 start = time_now_seconds();
    network_receive_data(state->network);
    f64 end = time_now_seconds();

    if (end - start > 0.5) {
      if (network_receive_game_over_message(network))

        state->stage = GAME_OVER_STAGE;
      break;
    }

    client_update(state);
    window_update(state->window);
    window_render(state->window);
    rendering_render(state->rendering);
    ui_render_game_ui(state->ui);
    window_swapbuffers(state->window);
    break;
  }
  case RETRY_STAGE: {
    window_input(state->window);
    window_render(state->window);
    ui_render_retry_screen(state->ui, state->match_result ? "won" : "lost");
    window_swapbuffers(state->window);

    state->running &= !window->should_close;
    break;
  }
  case GAME_OVER_STAGE: {
    window_input(state->window);
    window_render(state->window);
    ui_render_game_over_screen(state->ui);
    window_swapbuffers(state->window);

    state->running &= !window->should_close;
    break;
  }
  }
}

void
client_destroy(const Client_State* state) {
  ui_unload(state->ui);
  window_destroy(state->window);
}