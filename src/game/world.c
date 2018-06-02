#include "world.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_POINTS 3

// note: pixels/second
#define BALL_START_SPEED 10.0f

void
world_reset(World* world) {
  world->player_width = 20;
  world->player_height = 80;

  world->player_1.velocity = 0;
  world->player_2.velocity = 0;
  world->player_1.colliding = false;
  world->player_2.colliding = false;

  f32 ball_velocity_x =
      1.0 -
      max(min((((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f), 0.2f), -0.2);

  // note: limit y direction, so we do not start too steep
  f32 ball_velocity_y =
      max(min((((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f), 0.3f), -0.3);

  world->ball.radius = 10;
  world->ball.pos = (v2){(f32)world->width / 2, (f32)world->height / 2};
  world->ball.velocity =
      mul2(norm2((v2){ball_velocity_x, ball_velocity_y}), BALL_START_SPEED);
}

void
world_init(World* world) {
  world->player_1.points = 0;
  world->player_2.points = 0;
  srand(time(0));
  f32 players_y = (f32)world->height * 0.5f - (f32)world->player_width * 0.5f;
  world->player_1.y = players_y;
  world->player_2.y = players_y;
  world_reset(world);
}

bool
collided(v2 aabb_min, v2 aabb_max, v2 ball_center, f32 ball_radius) {
  v2 closest_point_in_aabb = min2(max2(ball_center, aabb_min), aabb_max);
  v2 tangent = sub2(closest_point_in_aabb, ball_center);
  f32 distance_squared = dot2(tangent, tangent);
  return distance_squared < (ball_radius * ball_radius);
}

v2
get_collision_normal(v2 aabb_min, v2 aabb_max, v2 ball_center) {
  v2 closest_point_in_aabb = min2(max2(ball_center, aabb_min), aabb_max);
  v2 dir = sub2(ball_center, closest_point_in_aabb);
  return norm2(dir);
}

void
world_update(World* world) {
  Ball* ball = &world->ball;
  Player* player_1 = &world->player_1;
  Player* player_2 = &world->player_2;

  f32 ball_half_radius = ball->radius * 0.5;
  v2 player_size = (v2){world->player_width, world->player_height};

  v2 p1_min = (v2){0.0f, world->player_1.y};
  v2 p1_max = add2(p1_min, player_size);
  v2 p2_min = (v2){world->width - world->player_width, world->player_2.y};
  v2 p2_max = add2(p2_min, player_size);

  bool p1_collided = collided(p1_min, p1_max, ball->pos, ball->radius);
  bool p2_collided = collided(p2_min, p2_max, ball->pos, ball->radius);

  if (p1_collided) {
    if (!player_1->colliding) {
      v2 normal = get_collision_normal(p1_min, p1_max, ball->pos);

      ball->velocity = add2(reflect2(ball->velocity, normal),
                            (v2){0.0, player_1->velocity * 0.5f});

      player_1->colliding = true;
    }
  } else {
    player_1->colliding = false;
  }

  if (p2_collided) {
    if (!player_2->colliding) {
      v2 normal = get_collision_normal(p2_min, p2_max, ball->pos);
      ball->velocity = add2(reflect2(ball->velocity, normal),
                            (v2){0.0, player_2->velocity * 0.5f});
      player_2->colliding = true;
    }
  } else {
    player_2->colliding = false;
  }

  float top = world->height - ball_half_radius;
  float bottom = ball_half_radius;

  if (ball->pos.y >= top) {
    ball->velocity = reflect2(ball->velocity, (v2){0.0, -1.0});
  } else if (ball->pos.y <= bottom) {
    ball->velocity = reflect2(ball->velocity, (v2){0.0, 1.0});
  }

  if (ball->pos.x < 0) {
    world->player_2.points++;
    world_reset(world);
  } else if (ball->pos.x > world->width) {
    world->player_1.points++;
    world_reset(world);
  } else
    ball->pos = add2(ball->pos, ball->velocity);

  player_1->velocity = 0;
  player_2->velocity = 0;
}

void
world_render(World* world) {}