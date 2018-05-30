#include <GLFW/glfw3.h>
#include "../game/input.h"

void
input_update(Input* input) {
  GLFWwindow* window = (GLFWwindow*)input->window->handle;
  void* state = glfwGetWindowUserPointer(window);

  i32 player1_up = glfwGetKey(window, GLFW_KEY_W);
  i32 player1_down = glfwGetKey(window, GLFW_KEY_S);
  i32 player2_up = glfwGetKey(window, GLFW_KEY_UP);
  i32 player2_down = glfwGetKey(window, GLFW_KEY_DOWN);

  if (player1_up == GLFW_PRESS)
    input->key_press(state, PLAYER1_UP);
  else if (player1_down == GLFW_PRESS)
    input->key_press(state, PLAYER1_DOWN);
  else if (player2_up == GLFW_PRESS)
    input->key_press(state, PLAYER2_UP);
  else if (player2_down == GLFW_PRESS)
    input->key_press(state, PLAYER2_DOWN);
  else
    input->key_press(state, NO_INPUT);
}