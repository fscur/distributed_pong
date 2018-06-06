#pragma once
#include "types.h"
#include "bitmap.h"
#include "window.h"
#include "world.h"

typedef struct Ui {
  Game_Window* window;
  World* world;
  void* state;
  Color clear_color;
  f32 time;
  f32 dt;
  char name[MAX_PLAYER_NAME_LENGTH];
  char server_ip[SERVER_IP_LENGTH];
  void (*play)(void* state,
               char name[MAX_PLAYER_NAME_LENGTH],
               char server_ip[SERVER_IP_LENGTH]);
  void (*close)(void* state);
} Ui;

void ui_init(Ui* ui);
void ui_load(Ui* ui);
void ui_render_game_ui(Ui* ui);
void ui_render_start_screen(Ui* ui);
void ui_render_searching_server(Ui* ui);
void ui_render_awaiting_challenger(Ui* ui);
void ui_render_retry_screen(Ui* ui, char* result);
void ui_unload(Ui* ui);
