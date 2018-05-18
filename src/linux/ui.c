#include "../game/ui.h"
#include "ui/imgui_impl_glfw_gl3.c"

typedef struct ImVec4 ImColor;
typedef struct ImVec2 ImVec2;
typedef struct ImVec4 ImVec4;
typedef struct ImGuiIO ImGuiIO;

internal void
update_gl_texture(u32 texID, Bitmap* image) {
  glBindTexture(GL_TEXTURE_2D, texID);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               image->width,
               image->height,
               0,
               GL_BGRA,
               GL_UNSIGNED_BYTE,
               image->data);
}

internal void
render_debug(Ui* ui) {
  ImGuiIO* io = igGetIO();
  igText("average: %.3f ms/frame (%.1f FPS)",
         1000.0f / io->Framerate,
         io->Framerate);

  igText("width: %d height: %d", ui->window->width, ui->window->height);
}

internal void
render_imgui(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  // bool open = true;
  // igShowDemoWindow(&open);

  render_debug(ui);

  igRender();
}

internal void
init_imgui(Ui* ui) {
  ImGui_ImplGlfwGL3_Init(ui->window->handle, true);

  struct ImGuiStyle* style = igGetStyle();
  igStyleColorsDark(style);

  struct ImGuiIO* io = igGetIO();
  ImFontAtlas_AddFontFromFileTTF(
      io->Fonts, "../res/fonts/UbuntuMono-Regular.ttf", 16.0f, 0, 0);
}

internal u32
create_texture(Bitmap* image) {
  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               image->width,
               image->height,
               0,
               GL_BGRA,
               GL_UNSIGNED_BYTE,
               image->data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  return texture_id;
}

void
ui_init(Ui* ui) {}

void
ui_load(Ui* ui) {
  init_imgui(ui);
}

void
ui_render(Ui* ui) {
  render_imgui(ui);
}

void
ui_unload(Ui* ui) {
  ImGui_ImplGlfwGL3_Shutdown();
}