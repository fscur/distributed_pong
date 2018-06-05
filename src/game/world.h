#pragma once
#include "types.h"

#define MAX_POINTS 20

// note: pixels/second
#define BALL_START_SPEED 10.0f

typedef struct Player {
  char name[MAX_PLAYER_NAME_LENGTH];
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
  Player players[MAX_PLAYER_COUNT];
  Ball ball;
  void (*on_goal)(void* world, Player* player);
} World;

typedef struct World_Rendering_Data {
  f32 width;
  f32 height;
  f32 player_width;
  f32 player_height;

  f32 time;
  f32 dt;
  v2 ball_position;

  f32 ball_radius;
  f32 player_1_y;
  f32 player_2_y;
  f32 pad1;

} World_Rendering_Data;

void world_init(World* world);
void world_update(World* world);