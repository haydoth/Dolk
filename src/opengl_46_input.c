#include "input.h"
#include "window.h"

#include <GLFW/glfw3.h>

int GetKey(int key) {
  return glfwGetKey((GLFWwindow*)GetWindowHandle(), key) == GLFW_PRESS;
}
int GetKeyUp(int key) {
  return 0;
}
int GetKeyDown(int key) {
  return 0;
}

void CaptureCursor(int doCapture) {
  
}

int isCursorCaptured() {
  return 0;
}
