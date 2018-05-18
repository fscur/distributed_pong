#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#pragma pack(push, 1) // pack struct bits

typedef struct BitmapHeader {
  u16 type;
  u32 file_size;
  u32 reserved0;
  u32 offset;
  u32 size;
  u32 width;
  i32 height;
  u16 planes;
  u16 bit_count;
  u32 compression;
  u32 data_size;
  i32 x_pels_per_meter;
  i32 y_pels_per_meter;
  u32 colors_used;
  u32 colors_important;
  u32 red_mask;
  u32 green_mask;
  u32 blue_mask;
  u32 alpha_mask;
  u32 cs_type;
  u32 cie_red_x;
  u32 cie_red_y;
  u32 cie_red_z;
  u32 cie_green_x;
  u32 cie_green_y;
  u32 cie_green_z;
  u32 cie_blue_x;
  u32 cie_blue_y;
  u32 cie_blue_z;
  u32 gamma_red;
  u32 gamma_green;
  u32 gamma_blue;
} BitmapHeader;

#pragma pack(pop)

typedef struct Bitmap {
  BitmapHeader header;
  u32 width;
  u32 height;
  u8* data;
} Bitmap;

Bitmap* bitmap_create_image(const u32 width, const i32 height);
void bitmap_clear_image(const Bitmap* image, const Color color);
Bitmap* bitmap_load_image(const char* file_name);
void bitmap_save_image(const Bitmap* image, char* file_name);
void bitmap_flip_image(Bitmap* image);
void bitmap_destroy_image(Bitmap* image);
