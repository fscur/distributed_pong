#pragma once
#include "types.h"
#include "bitmap.h"
#include "window.h"

typedef struct Ui {
  Game_Window* window;
  Bitmap* image;
  u32 texture_id;
  Color clear_color;
  f32 dt;
} Ui;

void ui_init(Ui* ui);
void ui_load(Ui* ui);
void ui_render(Ui* ui);
void ui_unload(Ui* ui);
