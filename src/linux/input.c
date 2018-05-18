#include <GLFW/glfw3.h>
#include "../game/input.h"

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // Game_State* state = glfwGetWindowUserPointer(window);
  // Input* input = (Input*)state->input;
  // if (action == GLFW_PRESS) {
  //   switch (key) {
  //   case GLFW_KEY_W:
  //     input->key_press(state, PLAYER1_UP);
  //     break;
  //   case GLFW_KEY_S:
  //     input->key_press(state, PLAYER1_DOWN);
  //     break;
  //   case GLFW_KEY_UP:
  //     input->key_press(state, PLAYER2_UP);
  //     break;
  //   case GLFW_KEY_DOWN:
  //     input->key_press(state, PLAYER2_DOWN);
  //     break;
  //   }
  // }
}

void
input_load(Input* input) {
  glfwSetKeyCallback(input->window->handle, key_callback);
}

void
input_update(Input* input) {
  GLFWwindow* window = (GLFWwindow*)input->window->handle;
  Game_State* state = glfwGetWindowUserPointer(window);

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