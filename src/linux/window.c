#include <gl3w/GL/gl3w.c>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "../game/client.h"
#include "../game/window.h"

static void
error_callback(const int error, const char* description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

void
on_close(GLFWwindow* handle) {
  glfwSetWindowShouldClose(handle, 0);

  Client_State* state = glfwGetWindowUserPointer(handle);
  Game_Window* window = state->window;
  window->on_close(state);
}

void
window_create(void* state, Game_Window* window) {
  // todo: how to create window with app state????
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    return;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window->handle = (void*)glfwCreateWindow(
      window->width, window->height, window->title, NULL, NULL);

  glfwMakeContextCurrent(window->handle);
  glfwSwapInterval(0);
  gl3wInit();

  glfwSetWindowUserPointer(window->handle, state);

  GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
  GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);
  glfwSetWindowPos(window->handle,
                   (video_mode->width - window->width) / 2,
                   (video_mode->height - window->height) / 2);

  glfwSetWindowCloseCallback(window->handle, on_close);
}

void
window_input(Game_Window* window) {
  glfwPollEvents();
}

void
window_update(Game_Window* window) {
  window->should_close = glfwWindowShouldClose(window->handle);
  glfwGetFramebufferSize(window->handle, &window->width, &window->height);
}

void
window_render(Game_Window* window) {
  glViewport(0, 0, window->width, window->height);
  glClearColor(window->back_color.r,
               window->back_color.g,
               window->back_color.b,
               window->back_color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void
window_swapbuffers(Game_Window* window) {
  glfwSwapBuffers(window->handle);
}

void
window_destroy(Game_Window* window) {
  glfwTerminate();
}