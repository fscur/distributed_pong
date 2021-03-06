#include "../game/ui.h"
#include "ui/imgui_impl_glfw_gl3.c"

typedef struct ImVec4 ImColor;
typedef struct ImVec2 ImVec2;
typedef struct ImVec4 ImVec4;
typedef struct ImGuiIO ImGuiIO;

#define UI_BUTTON_WIDTH 100
#define UI_BUTTON_HEIGHT 33
#define UI_HIGHLIGHT_COLOR                                                     \
  (ImVec4) { 0.95, 0.7, 0.2, 1.0 }
#define UI_FIXED_WINDOW                                                        \
  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |                    \
      ImGuiWindowFlags_NoMove

#define UI_TEXT_BOX_WIDTH 210

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
  style->Colors[ImGuiCol_WindowBg] = (ImVec4){0.06f, 0.05f, 0.07f, 0.60f};
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

  ImFontAtlas_AddFontFromFileTTF(
      io->Fonts, "../res/fonts/Ruda-Bold.ttf", 20.0f, 0, 0);
}

internal void
init_imgui(Ui* ui) {
  ImGui_ImplGlfwGL3_Init(ui->window->handle, true);

  struct ImGuiStyle* style = igGetStyle();
  set_style(style, ui);
}

void
ui_init(Ui* ui) {
  init_imgui(ui);
}

void
ui_render_start_screen(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("start_screen", &open, UI_FIXED_WINDOW);

  igPushStyleColor(ImGuiCol_Text, UI_HIGHLIGHT_COLOR);
  igText("Enter the server ip:");
  igPopStyleColor(1);

  static char server_ip[SERVER_IP_LENGTH] = LOCALHOST;
  igPushItemWidth(UI_TEXT_BOX_WIDTH);
  igPushIDInt(0);
  igInputText("", server_ip, SERVER_IP_LENGTH, 0, NULL, NULL);
  igPopID();
  igPopItemWidth(1);

  igPushStyleColor(ImGuiCol_Text, UI_HIGHLIGHT_COLOR);
  igText("Enter your name:");
  igPopStyleColor(1);

  static char name[MAX_PLAYER_NAME_LENGTH] = {};
  igPushItemWidth(UI_TEXT_BOX_WIDTH);
  igPushIDInt(1);
  igInputText("", name, MAX_PLAYER_NAME_LENGTH, 0, NULL, NULL);
  igPopID();
  igPopItemWidth();

  memcpy(ui->name, name, MAX_PLAYER_NAME_LENGTH);
  memcpy(ui->server_ip, server_ip, SERVER_IP_LENGTH);

  igPushIDInt(2);
  if (igButton("Play", (ImVec2){UI_BUTTON_WIDTH, UI_BUTTON_HEIGHT})) {
    ui->play(ui->state, name, server_ip);
  }
  igPopID(0);

  igSameLine(0, 10);
  igPushIDInt(3);
  if (igButton("Quit", (ImVec2){UI_BUTTON_WIDTH, UI_BUTTON_HEIGHT})) {
    ui->close(ui->state);
  }
  igPopID();
  igEnd();

  igRender();
}

void
ui_render_searching_server(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("searching_server", &open, UI_FIXED_WINDOW);

  igPushStyleColor(ImGuiCol_Text, UI_HIGHLIGHT_COLOR);
  igText("Searching server...");
  igPopStyleColor(1);
  igEnd();

  igRender();
}

void
ui_render_awaiting_challenger(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("awaiting", &open, UI_FIXED_WINDOW);

  igPushStyleColor(ImGuiCol_Text, UI_HIGHLIGHT_COLOR);
  igText("Awaiting challenger...");
  igPopStyleColor(1);
  igEnd();

  igRender();
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
render_score(Ui* ui) {
  bool open = true;
  igBegin("score", &open, ImGuiWindowFlags_NoTitleBar);
  igPushStyleColor(ImGuiCol_Text, (ImVec4){1.0, 0.0, 0.0, 1.0});
  igText("%s", ui->world->players[0].name);
  igPopStyleColor(1);
  igSameLine(0, 4);
  igPushStyleColor(ImGuiCol_Text, UI_HIGHLIGHT_COLOR);
  igText("%d X %d", ui->world->players[0].points, ui->world->players[1].points);
  igPopStyleColor(1);

  igSameLine(0, 4);
  igPushStyleColor(ImGuiCol_Text, (ImVec4){0.0, 0.0, 1.0, 1.0});
  igText("%s", ui->world->players[1].name);
  igPopStyleColor(1);
  igEnd();
}

void
ui_render_game_ui(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  // render_debug(ui);
  render_score(ui);

  igRender();
}

void
ui_render_retry_screen(Ui* ui, char* result) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("retry_screen", &open, UI_FIXED_WINDOW);
  igPushStyleColor(ImGuiCol_Text, UI_HIGHLIGHT_COLOR);
  igText("%s, you %s!\nWant to play again?", ui->name, result);

  igPopStyleColor(1);

  if (igButton("Play", (ImVec2){UI_BUTTON_WIDTH, UI_BUTTON_HEIGHT})) {
    ui->play(ui->state, ui->name, ui->server_ip);
  }

  igSameLine(0, 10);

  if (igButton("Quit", (ImVec2){UI_BUTTON_WIDTH, UI_BUTTON_HEIGHT})) {
    ui->close(ui->state);
  }
  igEnd();

  igRender();
}

void
ui_render_game_over_screen(Ui* ui) {
  ImGui_ImplGlfwGL3_NewFrame();

  bool open = true;
  igBegin("game_over_screen", &open, UI_FIXED_WINDOW);
  igPushStyleColor(ImGuiCol_Text, UI_HIGHLIGHT_COLOR);
  igText("%s, your opponent quit!\nWant to play again?", ui->name);

  igPopStyleColor(1);

  if (igButton("Play", (ImVec2){UI_BUTTON_WIDTH, UI_BUTTON_HEIGHT})) {
    ui->play(ui->state, ui->name, ui->server_ip);
  }

  igSameLine(0, 10);

  if (igButton("Quit", (ImVec2){UI_BUTTON_WIDTH, UI_BUTTON_HEIGHT})) {
    ui->close(ui->state);
  }
  igEnd();

  igRender();
}

void
ui_unload(Ui* ui) {
  ImGui_ImplGlfwGL3_Shutdown();
}