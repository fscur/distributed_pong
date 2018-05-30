#include "client.h"
#include "memory.c"
#include "bitmap.c"
#include "world.c"

// note: pixels/second
#define MOVE_SPEED 10.0f

#define START_STAGE 0
#define SEARCHING_SERVER_STAGE 1
#define AWAITING_STAGE 2
#define PLAYING_STAGE 3
#define RETRY_STAGE 4

void
stage0_play(Client_State* state, char name[MAX_PLAYER_NAME_LENGTH]) {
  sprintf(&state->name, "%s", name);
  state->stage = SEARCHING_SERVER_STAGE;
}

void
stage0_close(Client_State* state) {
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
  state->world = arena_push_struct(&arena, World);
  state->rendering = arena_push_struct(&arena, Render_State);
  state->network = arena_push_struct(&arena, Network_State);
  state->memory = memory;
  state->running = true;
  state->finished = false;
  state->dt = 0.0;
  state->stage = 0;

  Color color = (Color){0.07f, 0.07f, 0.09f, 1.00f};
  // Color color = (Color){0.08f, 0.09f, 0.12f, 1.00f};

  Game_Window* window = state->window;
  window->title = "pong";
  window->width = 1280;
  window->height = 720;
  window->back_color = color;

  Input* input = state->input;
  input->window = state->window;
  input->key_press = client_key_press;

  Ui* ui = state->ui;
  ui->state = state;
  ui->window = state->window;
  ui->world = state->world;
  ui->play = stage0_play;
  ui->close = stage0_close;

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
  world_init(world);

  Render_State* render = (Render_State*)state->rendering;
  render->uniform_buffer_data = state->world;
  render->uniform_buffer_data_size = sizeof(World);
  rendering_init(render);

  Network_State* network = (Network_State*)state->network;
  network_init_client(network);
}

void
client_load(Client_State* state) {
  ui_load(state->ui);
}

void
client_run(Client_State* state) {
  switch (state->stage) {
  case START_STAGE: {
    window_input(state->window);
    window_render(state->window);
    ui_render_start_screen(state->ui);
    window_swapbuffers(state->window);
    break;
  }
  case SEARCHING_SERVER_STAGE: {
    window_input(state->window);
    window_render(state->window);
    ui_render_searching_server(state->ui);
    window_swapbuffers(state->window);
    if (network_connect_to_server(state->network, state->name))
      state->stage = AWAITING_STAGE;
    break;
  }
  case AWAITING_STAGE: {
    window_input(state->window);
    window_render(state->window);
    ui_render_awaiting_challenger(state->ui);
    window_swapbuffers(state->window);

    if (network_receive_ready_message(state->network))
      state->stage = PLAYING_STAGE;

    break;
  }
  case PLAYING_STAGE: {
    window_input(state->window);
    input_update(state->input);
    network_receive_data(state->network);
    client_update(state);
    window_render(state->window);
    rendering_render(state->rendering);
    ui_render(state->ui);
    window_swapbuffers(state->window);
    break;
  }
  case RETRY_STAGE: {
    window_input(state->window);
    window_render(state->window);
    ui_render_retry_screen(state->ui);
    window_swapbuffers(state->window);
    break;
  }
  }
}

void
client_update(Client_State* state) {
  Game_Window* window = (Game_Window*)state->window;
  World* world = (World*)state->world;
  Network_State* network = state->network;
  Ui* ui = (Ui*)state->ui;

  world->ball.pos = network->current_packet.ball_pos;
  world->ball.radius = network->current_packet.ball_radius;
  world->player_1.y = network->current_packet.players_positions[0];
  world->player_2.y = network->current_packet.players_positions[1];
  world->player_1.points = network->current_packet.players_points[0];
  world->player_2.points = network->current_packet.players_points[1];

  ui->time = state->time;
  ui->dt = state->dt;
  world->time = state->time;
  world->dt = state->dt;
  world->height = window->height;
  world->width = window->width;

  state->running = !window->should_close;

  window_update(state->window);
}

void
client_unload(const Client_State* state) {
  ui_unload(state->ui);
}

void
client_destroy(const Client_State* state) {
  window_destroy(state->window);
}