#pragma once
#include "types.h"

typedef struct Player {
  f32 y;
  f32 velocity;
  i32 points;
  bool colliding;
} Player;

typedef struct Ball {
  v2 pos;
  v2 velocity;
  f32 radius;
} Ball;

typedef struct World {
  f32 time;
  f32 dt;
  i32 width;
  i32 height;
  i32 player_width;
  i32 player_height;
  f32 pad0;
  f32 pad1;
  Player player_1;
  Player player_2;
  Ball ball;
} World;

void world_init(World* world);
void world_update(World* world);
void world_render(World* world);