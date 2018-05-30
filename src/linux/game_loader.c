#include "lib_loader.c"
#include "../game/game.h"

#define GAME_LIB_NAME "lib_game.so"
#define TMP_GAME_LIB_NAME "tmp_lib_game.so"

void
load_game_code(Game_Code* code) {
  code->create = (Game_State * (*)(Memory*))fn(code->lib_handle, "game_create");
  code->init = (void (*)(Game_State*))fn(code->lib_handle, "game_init");
  code->load = (void (*)(Game_State*))fn(code->lib_handle, "game_load");
  code->input = (void (*)(Game_State*))fn(code->lib_handle, "game_input");
  code->input_client =
      (void (*)(Game_State*))fn(code->lib_handle, "game_input_client");

  code->update = (void (*)(Game_State*))fn(code->lib_handle, "game_update");
  code->update_client =
      (void (*)(Game_State*))fn(code->lib_handle, "game_update_client");
  code->render = (void (*)(Game_State*))fn(code->lib_handle, "game_render");
  code->connect_to_server =
      (void (*)(Game_State*))fn(code->lib_handle, "game_connect_to_server");
  code->receive_data =
      (void (*)(Game_State*))fn(code->lib_handle, "game_receive_data");
  code->wait_players =
      (void (*)(Game_State*))fn(code->lib_handle, "game_wait_players");
  code->broadcast =
      (void (*)(Game_State*))fn(code->lib_handle, "game_broadcast");
  code->unload = (void (*)(Game_State*))fn(code->lib_handle, "game_unload");
  code->destroy = (void (*)(Game_State*))fn(code->lib_handle, "game_destroy");
}

Game_Code
load_game() {
  Game_Code code = {};
  code.lib_handle = load_lib(GAME_LIB_NAME);
  load_game_code(&code);
  return code;
}

void
reload_game(Game_Code* code) {
  code->lib_handle =
      reload_lib(code->lib_handle, GAME_LIB_NAME, TMP_GAME_LIB_NAME);
  load_game_code(code);
}