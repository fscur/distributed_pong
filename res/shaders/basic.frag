#version 330 core

struct World {
  float time;
  float dt;
  ivec2 res;
  ivec2 player_size;
  float pad1;
  float pad2;
};

struct Player {
  float y;
  float velocity;
  float points;
  bool colliding;
};

struct Ball {
  vec2 position;
  vec2 velocity;
  float radius;
  float pad1;
  float pad2;
  float pad3;
};

layout(std140) uniform Game {
  World world;
  Player player1;
  Player player2;
  Ball ball;
};

out vec4 frag_color;

float
sphere(vec2 pos, vec2 center, float radius) {
  return 1.0 - smoothstep(radius - 0.05, radius + 0.05, length(pos - center));
}

float
box(vec2 pos, vec2 location, vec2 size) {

  vec2 bl = 1.0 - step(pos, location);
  vec2 tr = 1.0 - step(location + size, pos);

  return bl.x * bl.y * tr.x * tr.y;
}

float
vertical_line(vec2 pos, float line, float size) {
  return 1.0 -
         smoothstep(
             size,
             size + 0.01,
             length(max(abs(pos - vec2(line, pos.y)) - vec2(size, 0.0), 0.0)));
}

float
dotted_vertical_line(vec2 pos, float line, float size) {

  float l =
      1.0 -
      smoothstep(
          size,
          size + 2.0,
          length(max(abs(pos - vec2(line, pos.y)) - vec2(size, 0.0), 0.0)));

  float s = smoothstep(0.0, 0.01, sin((3.141569 * pos.y) / (size * 5.0)));
  return s * l;
}

vec4
over(vec4 a, vec4 b) {
  return a + b * (1.0 - a.w);
}

void
main() {

  vec2 res = vec2(float(world.res));
  float aspect = res.x / res.y;
  vec2 frag_pos = gl_FragCoord.xy / res;
  frag_pos.x *= aspect;
  frag_pos *= res.y;
  // frag_color = vec4(sin(world.time), sin(world.time), 1.0, 1.0);

  float half_width = world.res.x * 0.5;
  float half_height = world.res.y * 0.5;
  vec2 p1_pos = vec2(0.0, float(player1.y));
  vec2 p2_pos = vec2(world.res.x - world.player_size.x, float(player2.y));
  float p1 = box(frag_pos, p1_pos, world.player_size);
  float p2 = box(frag_pos, p2_pos, world.player_size);

  float b = sphere(frag_pos, ball.position, ball.radius);
  float l = dotted_vertical_line(frag_pos, half_width, 2.0);

  vec4 world = vec4(l);
  world = over(vec4(p1, 0.0, p2, max(p1, p2)), world);
  world = over(vec4(0, b, 0, b), world);
  frag_color = world;
}