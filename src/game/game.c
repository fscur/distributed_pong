#include "game.h"
#include "memory.c"
#include "bitmap.c"

#define MOVE_STEP 0.1f

internal void
key_press(Game_State* state, i32 key) {
  World* world = (World*)state->world;
  Game_Window* window = (Game_Window*)state->window;
  // window->back_color = (Color){0.08f, 0.7f, 0.12f, 1.00f};

  switch (key) {
  case PLAYER1_UP:
    if (world->player_1.y < window->height)
      world->player_1.y += MOVE_STEP;
    break;
  case PLAYER1_DOWN:
    if (world->player_1.y > 0)
      world->player_1.y -= MOVE_STEP;
    break;
  case PLAYER2_UP:
    if (world->player_2.y < window->height)
      world->player_2.y += MOVE_STEP;
    break;
  case PLAYER2_DOWN:
    if (world->player_2.y > 0)
      world->player_2.y -= MOVE_STEP;
    break;
  }
}

Game_State*
game_create(Memory* memory) {
  assert(sizeof(Game_State) <= memory->permanent_size);

  Memory_Arena arena = {};
  arena_init(&arena, memory->permanent_addr, memory->permanent_size);

  Game_State* state = arena_push_struct(&arena, Game_State);
  state->window = arena_push_struct(&arena, Game_Window);
  state->input = arena_push_struct(&arena, Input);
  state->ui = arena_push_struct(&arena, Ui);
  state->world = arena_push_struct(&arena, World);
  state->memory = memory;
  state->running = true;
  state->dt = 0.0;

  Color color = (Color){0.08f, 0.09f, 0.12f, 1.00f};

  Game_Window* window = state->window;
  window->title = "rays and chains";
  window->width = 1280;
  window->height = 720;
  window->back_color = color;

  Input* input = state->input;
  input->window = state->window;
  input->key_press = key_press;
  Ui* ui = state->ui;
  ui->window = state->window;

  return state;
}

void
game_init(Game_State* state) {
  ui_init(state->ui);
}

void
game_load(Game_State* state) {
  ui_load(state->ui);
  input_load(state->input);
}

void
game_input(Game_State* state) {
  input_update(state->input);
  window_input(state->window);
}

void
game_update(Game_State* state) {
  Game_Window* window = (Game_Window*)state->window;
  World* world = (World*)state->world;

  float y1 = world->player_1.y / (float)window->height;
  float y2 = world->player_2.y / (float)window->height;

  window->back_color = (Color){y1, y2, 0.0, 1.0};

  state->running = !window->should_close;
  window_update(state->window);
}

void
game_render(const Game_State* state) {
  window_render(state->window);
  ui_render(state->ui);
  window_swapbuffers(state->window);
}

void
game_unload(const Game_State* state) {
  ui_unload(state->ui);
}

void
game_destroy(const Game_State* state) {
  window_destroy(state->window);
}