#include <GLFW/glfw3.h>
#include "../game/input.h"

void
input_update(Input* input) {
  GLFWwindow* window = (GLFWwindow*)input->window->handle;
  void* state = glfwGetWindowUserPointer(window);

  int action = glfwGetKey(window, GLFW_KEY_W);
  if (action == GLFW_PRESS)
    input->key_press(state, PLAYER1_UP);

  action = glfwGetKey(window, GLFW_KEY_S);
  if (action == GLFW_PRESS)
    input->key_press(state, PLAYER1_DOWN);

  action = glfwGetKey(window, GLFW_KEY_UP);
  if (action == GLFW_PRESS)
    input->key_press(state, PLAYER2_UP);

  action = glfwGetKey(window, GLFW_KEY_DOWN);
  if (action == GLFW_PRESS)
    input->key_press(state, PLAYER2_DOWN);
}