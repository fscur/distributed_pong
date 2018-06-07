#include "world.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void
world_reset(World* world) {
  world->players[0].velocity = 0;
  world->players[1].velocity = 0;
  world->players[0].colliding = false;
  world->players[1].colliding = false;

  // note: limit x,y direction, so we do not start too steep
  f32 flip = (f32)(rand() % 2) * 2.0 - 1.0;
  f32 ball_velocity_x = (0.5 + ((f32)rand() / (f32)RAND_MAX) * 0.5) * flip;
  f32 ball_velocity_y = (((f32)rand() / (f32)RAND_MAX) * 0.5) * 2.0 - 0.5;

  printf("%f - %f - %f\n", ball_velocity_x, ball_velocity_y, flip);

  world->ball.radius = 10;
  world->ball.pos = (v2){(f32)world->width / 2, (f32)world->height / 2};
  world->ball.velocity =
      mul2(norm2((v2){ball_velocity_x, ball_velocity_y}), BALL_START_SPEED);
}

void
world_init(World* world) {

  f32 players_y = (f32)world->height * 0.5f - (f32)world->player_height * 0.5f;
  world->player_width = 20;
  world->player_height = 80;
  world->players[0].points = 0;
  world->players[1].points = 0;
  world->players[0].y = players_y;
  world->players[1].y = players_y;
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

  if (dir.x == 0 && dir.y == 0) {
    f32 minDist = 10000.0;
    if (fabs(ball_center.x - aabb_max.x) < minDist) {
      minDist = fabs(ball_center.x - aabb_max.x);
      dir = (v2){1.0, 0.0};
    }
    if (fabs(ball_center.x - aabb_min.x) < minDist) {
      minDist = fabs(ball_center.x - aabb_min.x);
      dir = (v2){-1.0, 0.0};
    }
    if (fabs(ball_center.y - aabb_max.y) < minDist) {
      minDist = fabs(ball_center.y - aabb_max.y);
      dir = (v2){0.0, 1.0};
    }
    if (fabs(ball_center.y - aabb_min.y) < minDist) {
      minDist = fabs(ball_center.y - aabb_min.y);
      dir = (v2){0.0, -1.0};
    }
  }

  return norm2(dir);
}

void
world_update(World* world) {
  Ball* ball = &world->ball;
  Player* player_1 = &world->players[0];
  Player* player_2 = &world->players[1];

  f32 ball_half_radius = ball->radius * 0.5;
  v2 player_size = (v2){world->player_width, world->player_height};

  v2 p1_min = (v2){0.0f, player_1->y};
  v2 p1_max = add2(p1_min, player_size);
  v2 p2_min = (v2){world->width - world->player_width, player_2->y};
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
    player_2->points++;
    world->on_goal(world, player_2);
    world_reset(world);
  } else if (ball->pos.x > world->width) {
    player_1->points++;
    world->on_goal(world, player_1);
    world_reset(world);
  } else
    ball->pos = add2(ball->pos, ball->velocity);

  player_1->velocity = 0;
  player_2->velocity = 0;
}