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
  igText("w: %d", ui->window->width);
  igText("h: %d", ui->window->height);
  igText("t: %.3f seconds", ui->time);
  igText("   %.3f ms/frame", ui->dt * 1000.0f);
  igText("   %.3f frames/s", 1.0f / ui->dt);
}

internal void
render_placar(Ui* ui) {
  bool open = true;
  igBegin("game", &open, ImGuiWindowFlags_NoTitleBar);
  igText("%d X %d", ui->world->player_1.points, ui->world->player_2.points);
  igEnd();
}

internal void
render_imgui(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  render_debug(ui);
  render_placar(ui);

  igRender();
}

void
set_style(struct ImGuiStyle* style, Ui* ui) {
  style->WindowPadding = (ImVec2){15, 15};
  style->WindowRounding = 2.0f;
  style->FramePadding = (ImVec2){5, 5};
  style->FrameRounding = 1.0f;
  style->ItemSpacing = (ImVec2){12, 8};
  style->ItemInnerSpacing = (ImVec2){8, 6};
  style->IndentSpacing = 25.0f;
  style->ScrollbarSize = 15.0f;
  style->ScrollbarRounding = 9.0f;
  style->GrabMinSize = 5.0f;
  style->GrabRounding = 3.0f;

  style->Colors[ImGuiCol_Text] = (ImVec4){0.80f, 0.80f, 0.83f, 1.00f};
  style->Colors[ImGuiCol_TextDisabled] = (ImVec4){0.24f, 0.23f, 0.29f, 1.00f};
  style->Colors[ImGuiCol_WindowBg] = (ImVec4){0.06f, 0.05f, 0.07f, 1.00f};
  style->Colors[ImGuiCol_ChildBg] = (ImVec4){0.07f, 0.05f, 0.07f, 1.00f};
  style->Colors[ImGuiCol_PopupBg] = (ImVec4){0.07f, 0.07f, 0.09f, 1.00f};
  style->Colors[ImGuiCol_Border] = (ImVec4){0.80f, 0.80f, 0.83f, 0.88f};
  style->Colors[ImGuiCol_BorderShadow] = (ImVec4){0.92f, 0.91f, 0.88f, 0.00f};
  style->Colors[ImGuiCol_FrameBg] = (ImVec4){0.10f, 0.09f, 0.12f, 1.00f};
  style->Colors[ImGuiCol_FrameBgHovered] = (ImVec4){0.24f, 0.23f, 0.29f, 1.00f};
  style->Colors[ImGuiCol_FrameBgActive] = (ImVec4){0.56f, 0.56f, 0.58f, 1.00f};
  style->Colors[ImGuiCol_TitleBg] = (ImVec4){0.10f, 0.09f, 0.12f, 1.00f};
  style->Colors[ImGuiCol_TitleBgCollapsed] =
      (ImVec4){1.00f, 0.98f, 0.95f, 0.75f};
  style->Colors[ImGuiCol_TitleBgActive] = (ImVec4){0.07f, 0.07f, 0.09f, 1.00f};
  style->Colors[ImGuiCol_MenuBarBg] = (ImVec4){0.10f, 0.09f, 0.12f, 1.00f};
  style->Colors[ImGuiCol_ScrollbarBg] = (ImVec4){0.10f, 0.09f, 0.12f, 1.00f};
  style->Colors[ImGuiCol_ScrollbarGrab] = (ImVec4){0.80f, 0.80f, 0.83f, 0.31f};
  style->Colors[ImGuiCol_ScrollbarGrabHovered] =
      (ImVec4){0.56f, 0.56f, 0.58f, 1.00f};
  style->Colors[ImGuiCol_ScrollbarGrabActive] =
      (ImVec4){0.06f, 0.05f, 0.07f, 1.00f};
  style->Colors[ImGuiCol_CheckMark] = (ImVec4){0.80f, 0.80f, 0.83f, 0.31f};
  style->Colors[ImGuiCol_SliderGrab] = (ImVec4){0.80f, 0.80f, 0.83f, 0.31f};
  style->Colors[ImGuiCol_SliderGrabActive] =
      (ImVec4){0.06f, 0.05f, 0.07f, 1.00f};
  style->Colors[ImGuiCol_Button] = (ImVec4){0.10f, 0.09f, 0.12f, 1.00f};
  style->Colors[ImGuiCol_ButtonHovered] = (ImVec4){0.24f, 0.23f, 0.29f, 1.00f};
  style->Colors[ImGuiCol_ButtonActive] = (ImVec4){0.56f, 0.56f, 0.58f, 1.00f};
  style->Colors[ImGuiCol_Header] = (ImVec4){0.10f, 0.09f, 0.12f, 1.00f};
  style->Colors[ImGuiCol_HeaderHovered] = (ImVec4){0.56f, 0.56f, 0.58f, 1.00f};
  style->Colors[ImGuiCol_HeaderActive] = (ImVec4){0.06f, 0.05f, 0.07f, 1.00f};
  style->Colors[ImGuiCol_ResizeGrip] = (ImVec4){0.00f, 0.00f, 0.00f, 0.00f};
  style->Colors[ImGuiCol_ResizeGripHovered] =
      (ImVec4){0.56f, 0.56f, 0.58f, 1.00f};
  style->Colors[ImGuiCol_ResizeGripActive] =
      (ImVec4){0.06f, 0.05f, 0.07f, 1.00f};
  style->Colors[ImGuiCol_CloseButton] = (ImVec4){0.40f, 0.39f, 0.38f, 0.16f};
  style->Colors[ImGuiCol_CloseButtonHovered] =
      (ImVec4){0.40f, 0.39f, 0.38f, 0.39f};
  style->Colors[ImGuiCol_CloseButtonActive] =
      (ImVec4){0.40f, 0.39f, 0.38f, 1.00f};
  style->Colors[ImGuiCol_PlotLines] = (ImVec4){0.40f, 0.39f, 0.38f, 0.63f};
  style->Colors[ImGuiCol_PlotLinesHovered] =
      (ImVec4){0.25f, 1.00f, 0.00f, 1.00f};
  style->Colors[ImGuiCol_PlotHistogram] = (ImVec4){0.40f, 0.39f, 0.38f, 0.63f};
  style->Colors[ImGuiCol_PlotHistogramHovered] =
      (ImVec4){0.25f, 1.00f, 0.00f, 1.00f};
  style->Colors[ImGuiCol_TextSelectedBg] = (ImVec4){0.25f, 1.00f, 0.00f, 0.43f};

  style->Colors[ImGuiCol_ModalWindowDarkening] =
      (ImVec4){1.00f, 0.98f, 0.95f, 0.73f};

  struct ImGuiIO* io = igGetIO();
  // ImFontAtlas_AddFontFromFileTTF(io->Fonts,
  // "../res/fonts/PressStart2P-Regular.ttf", 15.0f, 0, 0);
  ImFontAtlas_AddFontFromFileTTF(
      io->Fonts, "../res/fonts/Ruda-Bold.ttf", 16.0f, 0, 0);
}

internal void
init_imgui(Ui* ui) {
  ImGui_ImplGlfwGL3_Init(ui->window->handle, true);

  struct ImGuiStyle* style = igGetStyle();
  set_style(style, ui);
  // igStyleColorsDark(style);
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
ui_render_start_screen(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("start_screen",
          &open,
          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
  igPushStyleColor(ImGuiCol_Text, (ImVec4){0.95, 0.7, 0.2, 1.0});
  igText("Enter your name:");
  igPopStyleColor(1);
  igPushItemWidth(210);
  static char name[MAX_PLAYER_NAME_LENGTH] = {};
  igInputText("", name, 20, 0, NULL, NULL);
  igPopItemWidth();
  sprintf(ui->name, "%s", name);
  if (igButton("Play", (ImVec2){100, 27})) {
    ui->play(ui->state, name);
  }

  igSameLine(0, 10);

  if (igButton("Quit", (ImVec2){100, 27})) {
    ui->close(ui->state);
  }
  igEnd();

  igRender();
}

void
ui_render_searching_server(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("searching_server",
          &open,
          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

  igPushStyleColor(ImGuiCol_Text, (ImVec4){0.95, 0.7, 0.2, 1.0});
  igText("Searching server...");
  igPopStyleColor(1);
  igEnd();

  igRender();
}

void
ui_render_awaiting_challenger(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("awaiting",
          &open,
          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

  igPushStyleColor(ImGuiCol_Text, (ImVec4){0.95, 0.7, 0.2, 1.0});
  igText("Awaiting challenger...");
  igPopStyleColor(1);
  igEnd();

  igRender();
}

ui_render_retry_screen(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("retry_screen",
          &open,
          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
  igPushStyleColor(ImGuiCol_Text, (ImVec4){0.95, 0.7, 0.2, 1.0});
  igText("%s, you %s!\nWant to play again?", ui->name, "lost");

  igPopStyleColor(1);

  if (igButton("Play", (ImVec2){100, 27})) {
    ui->play(ui->state, ui->name);
  }

  igSameLine(0, 10);

  if (igButton("Quit", (ImVec2){100, 27})) {
    ui->close(ui->state);
  }
  igEnd();

  igRender();
}

void
ui_unload(Ui* ui) {
  ImGui_ImplGlfwGL3_Shutdown();
}