#pragma once
#include "shader_program.h"

typedef struct Render_State {
  u32 vao_id;
  u32 vertex_buffer_id;
  u32 index_buffer_id;
  u32 uniform_buffer_id;
  void* uniform_buffer_data;
  size_t uniform_buffer_data_size;
  Shader_Program program;
} Render_State;

void rendering_init(Render_State* state);
void rendering_render(Render_State* state);