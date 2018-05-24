#include <gl3w/GL/gl3w.c>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "../game/window.h"

static void
error_callback(const int error, const char* description) {
  fprintf(stderr, "Error %d: %s\n", error, description);
}

// static void
// key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
// {
//   Game_State* state = glfwGetWindowUserPointer(window);
//   Input* input = (Input*)state->input;
//   if (key == GLFW_KEY_E && action == GLFW_PRESS) {
//     input->key_press(state, GLFW_KEY_E);
//   }
// }

void
window_create(Game_State* state, Game_Window* window) {
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
  // glfwSetKeyCallback(window->handle, key_callback);
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

  // Color clear_color = window->back_color;
  // glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
  // glClear(GL_COLOR_BUFFER_BIT);
}

void
window_swapbuffers(Game_Window* window) {
  glfwSwapBuffers(window->handle);
}

void
window_destroy(Game_Window* window) {
  glfwTerminate();
}