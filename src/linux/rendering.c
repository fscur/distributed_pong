#include <gl3w/GL/gl3w.c>
#include <GLFW/glfw3.h>
#include "../game/rendering.h"
#include <stdio.h>
#include <string.h>

u32
load_shader(const char* file_name, u32 type) {

  size_t file_size = io_get_file_size(file_name);
  // todo: allocate with memory_arena
  char* code = malloc(file_size + 1);
  memset(code, '\0', file_size + 1);

  io_read_text_file(file_name, code);

  int info_log_length = 0;
  GLint result = GL_FALSE;
  GLuint id = glCreateShader(type);

  // Compile Vertex Shader
  printf("Compiling shader : %s\n", file_name);
  glShaderSource(id, 1, &code, NULL);
  glCompileShader(id);

  // Check Vertex Shader
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_log_length);

  if (info_log_length > 0) {
    char error_msg[info_log_length + 1];
    glGetShaderInfoLog(id, info_log_length, NULL, &error_msg);
    printf("%s\n", error_msg);
  }

  return id;
}

void
load_program(Shader_Program* program) {
  // Link the program
  printf("Linking program\n");
  GLuint id = glCreateProgram();
  glAttachShader(id, program->vertex_shader_id);
  glAttachShader(id, program->fragment_shader_id);
  glLinkProgram(id);

  GLint result = GL_FALSE;
  int info_log_length = 0;
  // Check the program
  glGetProgramiv(id, GL_LINK_STATUS, &result);
  glGetProgramiv(id, GL_INFO_LOG_LENGTH, &info_log_length);

  if (info_log_length > 0) {
    char error_msg[info_log_length + 1];
    glGetProgramInfoLog(id, info_log_length, NULL, &error_msg);
    printf("%s\n", error_msg);
  }

  glDetachShader(id, program->vertex_shader_id);
  glDetachShader(id, program->fragment_shader_id);

  glDeleteShader(program->vertex_shader_id);
  glDeleteShader(program->fragment_shader_id);
  program->id = id;
}

void
rendering_init(Render_State* state) {
  state->program.vertex_shader_id =
      load_shader("../res/shaders/basic.vert", GL_VERTEX_SHADER);

  state->program.fragment_shader_id =
      load_shader("../res/shaders/basic.frag", GL_FRAGMENT_SHADER);

  load_program(&state->program);

  glGenVertexArrays(1, &state->vao_id);
  glBindVertexArray(state->vao_id);

  static const GLfloat vertex_buffer_data[] = {
      -1.0f,
      -1.0f,
      0.0f,
      1.0f,
      -1.0f,
      0.0f,
      1.0f,
      1.0f,
      0.0f,
      1.0f,
      1.0f,
      0.0f,
      -1.0f,
      1.0f,
      0.0f,
      -1.0f,
      -1.0f,
      0.0f,
  };

  glGenBuffers(1, &state->vertex_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, state->vertex_buffer_id);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(vertex_buffer_data),
               vertex_buffer_data,
               GL_STATIC_DRAW);

  glBindVertexArray(0);

  u32 binding_point = 1;
  GLuint block_index = glGetUniformBlockIndex(state->program.id, "Game");
  glUniformBlockBinding(state->program.id, block_index, binding_point);

  glGenBuffers(1, &state->uniform_buffer_id);
  glBindBuffer(GL_UNIFORM_BUFFER, state->uniform_buffer_id);
  glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, state->uniform_buffer_id);
  glBufferData(GL_UNIFORM_BUFFER,
               state->uniform_buffer_data_size,
               state->uniform_buffer_data,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

#include "../game/world.h"

void
rendering_render(Render_State* state) {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindVertexArray(state->vao_id);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, state->vertex_buffer_id);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glUseProgram(state->program.id);
  glBindBuffer(GL_UNIFORM_BUFFER, state->uniform_buffer_id);

  // World* world = (World*)state->uniform_buffer_data;

  glBufferData(GL_UNIFORM_BUFFER,
               state->uniform_buffer_data_size,
               state->uniform_buffer_data,
               GL_DYNAMIC_DRAW);

  // GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

  // World* world = (World*)state->uniform_buffer_data;
  // memcpy(p, &state->uniform_buffer_data, state->uniform_buffer_data_size);
  // glUnmapBuffer(GL_UNIFORM_BUFFER);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glUseProgram(0);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
}