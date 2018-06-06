#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define local static
#define internal static

// primitive types are lower-case

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

#if _DEBUG
#define assert(expression)                                                     \
  if (!(expression)) {                                                         \
    *(int*)0 = 0;                                                              \
  }
#else
#define assert(expression)
#endif

#define kilobytes(Value) ((Value)*1024LL)
#define megabytes(Value) (kilobytes(Value) * 1024LL)
#define gigabytes(Value) (megabytes(Value) * 1024LL)
#define terabytes(Value) (gigabytes(Value) * 1024LL)

typedef struct v2 {
  f32 x;
  f32 y;
} v2;

typedef struct v3 {
  f32 x;
  f32 y;
  f32 z;
} v3;

typedef struct Color {
  f32 r;
  f32 g;
  f32 b;
  f32 a;
} Color;

#define MAX_PLAYER_COUNT 2
#define MAX_PLAYER_NAME_LENGTH 10
#define SERVER_IP_LENGTH 15