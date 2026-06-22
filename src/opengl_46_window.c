#include "window.h"

#include "arena.h"

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

global GLFWwindow* OpenGL_Window;

void*
GetWindowHandle() {
  return OpenGL_Window;
}

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

void
SetWindowTitle(const char* title) {
  glfwSetWindowTitle(OpenGL_Window, title);
}

void
SetWindowPosition(int x, int y) {
  glfwSetWindowPos(OpenGL_Window, x, y);
}

WindowPosition
GetWindowPosition() {
  int x, y;
  glfwGetWindowPos(OpenGL_Window, &x, &y);
  return (WindowPosition) {x, y};
}

void
SetWindowSize(int width, int height) {
  glfwSetWindowSize(OpenGL_Window, width, height);
}

WindowSize
GetWindowSize() {
  int w, h;
  glfwGetWindowSize(OpenGL_Window, &w, &h);
  return (WindowSize) {w, h};
}

typedef struct {
  int Hint, Value;
} OpenGL_WindowHint;

internal int
OpenGL_InitGLFW(OpenGL_WindowHint* hints, u64 hintCount) {
  if(!glfwInit()) {
    DOLK_ERROR("Failed to initialize GLFW!");
    return 0;
  }
  for(int i = 0; i < hintCount; ++i) {
    glfwWindowHint(hints[i].Hint, hints[i].Value);
  }
  return 1;
}

int
CreateWindow(const char* windowTitle, int windowWidth, int windowHeight)
{
  OpenGL_WindowHint hints[16] = {
    {GLFW_CONTEXT_VERSION_MAJOR, 4},
    {GLFW_CONTEXT_VERSION_MINOR, 6},
    {GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE}
  };
  if(!OpenGL_InitGLFW(hints, 3)) return 0;
  
  OpenGL_Window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, 0, 0);
  if(!OpenGL_Window) return 0;
  glfwMakeContextCurrent(OpenGL_Window);
  glfwSetFramebufferSizeCallback(OpenGL_Window, OpenGL_FramebufferResizeCallback);
  
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    DOLK_ERROR("Failed to initialize GLAD!");
    return 0;
  }
  glViewport(0, 0, windowWidth, windowHeight);

  return 1;
}
