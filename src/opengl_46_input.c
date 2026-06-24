#include "input.h"
#include "window.h"

// STD
#include <string.h>

// OpenGL
#include <GLFW/glfw3.h>

void
Input_EndFrame(InputState* state) {
  memcpy(state->lastKeyStates, state->currentKeyStates, sizeof(state->lastKeyStates));
  memcpy(state->lastMousePosition, state->currentMousePosition, sizeof(state->lastMousePosition));
}

int
KeyIsPressed(InputState* state, int key) {
  return state->currentKeyStates[key] == GLFW_PRESS;
}

int
KeyWasJustReleased(InputState* state, int key) {
  return state->currentKeyStates[key] == GLFW_RELEASE && state->lastKeyStates[key] == GLFW_PRESS;
}

int
KeyWasJustPressed(InputState* state, int key) {
  return state->currentKeyStates[key] == GLFW_PRESS && state->lastKeyStates[key] == GLFW_RELEASE;
}

void
ToggleMouseSeizure(void *windowHandle) {
  MouseIsSeized(windowHandle) ?
    glfwSetInputMode((GLFWwindow*)windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED) :
    glfwSetInputMode((GLFWwindow*)windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
int
MouseIsSeized(void *windowHandle) {
  return glfwGetInputMode((GLFWwindow*)windowHandle, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;  
}

MouseDelta
GetMouseDelta() {
  
}
MousePosition
GetMousePosition() {
  
}
