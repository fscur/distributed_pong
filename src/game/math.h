#pragma once
#include "types.h"
#include <math.h>
#include <stdbool.h>

u32
from_u8_to_32(u8 h1, u8 h0, u8 l1, u8 l0) {
  u32 result = (h1 << 24) | (h0 << 16) | (l1 << 8) | (l0 << 0);

  return result;
}

i32
round_f32_to_i32(f32 value) {
  return (i32)roundf(value);
}

f32
dot2(v2 a, v2 b) {
  return a.x * b.x + a.y * b.y;
}

f32
len2(v2 v) {
  return sqrt(dot2(v, v));
}

v3
add3(v3 a, v3 b) {
  return (v3){a.x + b.x, a.y + b.y, a.z + b.z};
}

v3
sub3(v3 a, v3 b) {
  return (v3){a.x - b.x, a.y - b.y, a.z - b.z};
}

v3
neg3(v3 v) {
  return (v3){-v.x, -v.y, -v.z};
}

v3
mul3(v3 v, f32 s) {
  return (v3){v.x * s, v.y * s, v.z * s};
}

v3
div3(v3 v, f32 s) {
  return (v3){v.x / s, v.y / s, v.z / s};
}

f32
dot3(v3 a, v3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

v3
cross3(v3 a, v3 b) {
  return (v3){
      a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y};
}

f32
len3(v3 v) {
  return sqrt(dot3(v, v));
}

v3
norm3(v3 v) {
  return div3(v, len3(v));
}