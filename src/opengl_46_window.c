#include "window.h"

#include "arena.h"
#include "input.h"

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//TODO: move clear color functions and glviewport to renderer
void
SetClearColor(float r, float g, float b) {
  glClearColor(r, g, b, 1.0f);
}

void
SetClearColorV(float* v) {
  glClearColor(v[0], v[1], v[2], 1.0f);
}

internal void
OpenGL_FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
  UNUSED(window);
  glViewport(0, 0, width, height);
}

internal void
OpenGL_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  UNUSED(mods);
  UNUSED(scancode);

  if(action == GLFW_REPEAT) return;
  
  WindowState *windowState = (WindowState*)glfwGetWindowUserPointer(window);
  windowState->input.currentKeyStates[key] = (char)action;
}

void
SetWindowTitle(WindowState* window, const char* title) {
  glfwSetWindowTitle((GLFWwindow*)window->handle, title);
}

void
SetWindowPosition(WindowState* window, int x, int y) {
  glfwSetWindowPos((GLFWwindow*)window->handle, x, y);
}

WindowPosition
GetWindowPosition(WindowState* window) {
  int x, y;
  glfwGetWindowPos((GLFWwindow*)window->handle, &x, &y);
  return (WindowPosition) {x, y};
}

void
SetWindowSize(WindowState* window, int width, int height) {
  glfwSetWindowSize((GLFWwindow*)window->handle, width, height);
}

WindowSize
GetWindowSize(WindowState* window) {
  int w, h;
  glfwGetWindowSize((GLFWwindow*)window->handle, &w, &h);
  return (WindowSize) {w, h};
}

int
CreateWindow(WindowState* window, const char* windowTitle, int windowWidth, int windowHeight)
{
  // Init GLFW

  if(!glfwInit()) {
    DOLK_ERROR("Failed to initialize GLFW!");
    return 0;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window->handle = glfwCreateWindow(windowWidth, windowHeight, windowTitle, 0, 0);
  if(!window->handle) return 0;
  glfwMakeContextCurrent((GLFWwindow*)window->handle);

  // Register GLFW callbacks
  glfwSetWindowUserPointer((GLFWwindow*)window->handle, window);
  glfwSetFramebufferSizeCallback((GLFWwindow*)window->handle, OpenGL_FramebufferResizeCallback);
  glfwSetKeyCallback((GLFWwindow*)window->handle, OpenGL_KeyCallback);
  
  // Init GLAD
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    DOLK_ERROR("Failed to initialize GLAD!");
    return 0;
  }
  glViewport(0, 0, windowWidth, windowHeight);
  
  return 1;
}

double GetElapsedTime() {
  return glfwGetTime();
}
