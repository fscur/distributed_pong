#include "types.h"
#include "io.h"

typedef struct Shader_Program {
  u32 id;
  u32 vertex_shader_id;
  u32 fragment_shader_id;
} Shader_Program;

u32 load_shader(const char* file_name, u32 type);
void load_program(Shader_Program* program);