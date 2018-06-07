#include <GLFW/glfw3.h>
#include "../game/input.h"

void
input_update(Input* input) {
  GLFWwindow* window = (GLFWwindow*)input->window->handle;
  void* state = glfwGetWindowUserPointer(window);

  i32 player_up = glfwGetKey(window, GLFW_KEY_UP);
  i32 player_down = glfwGetKey(window, GLFW_KEY_DOWN);

  if (player_up == GLFW_PRESS)
    input->key_press(state, PLAYER_UP);
  else if (player_down == GLFW_PRESS)
    input->key_press(state, PLAYER_DOWN);
  else
    input->key_press(state, NO_INPUT);
}