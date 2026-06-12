#include <AL/al.h>
#include <AL/alc.h>

// STD
#include <stdio.h>
#include <string.h>
#include <math.h>


// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Dolk
#include "common.h"
#include "arena.h"
#include "file_io.h"
#include "sv.h"

#include "dolk.h"

/*
  GAME ARCHITECTURE, courtesy of C. Muratori
  
  Chisel
     |
  Platform
     |
  OpenGL / D3D
  

  1 -------------|   Resources:     
  Input          |   - Audio         
  2 -------------|   - Video   
  Update         |   - Data   
  Render         |      
  SoundPrep      |   * Streaming vs loading screens     
  1 -------------|
  Sound Output   |   

  - Exploration-based architecture
  - Project State Space
    - like a map
    - similar to what J. Tyroller talks about
  
 */

/*

 */

global GLFWwindow* OpenGL_Window;

void
OpenGL_SetClearColor(float r, float g, float b) {
  glClearColor(r, g, b, 1.0f);
}

internal void
OpenGL_FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
  UNUSED(window);
  glViewport(0, 0, width, height);
}

void
OpenGL_SetWindowTitle(const char* title) {
  glfwSetWindowTitle(OpenGL_Window, title);
}

typedef struct opengl_window_pos { int X, Y; } opengl_window_pos;
typedef struct opengl_window_size { int Width, Height; } opengl_window_size;


void
OpenGL_SetWindowPosition(int x, int y) {
  glfwSetWindowPos(OpenGL_Window, x, y);
}

opengl_window_pos
OpenGL_GetWindowPosition() {
  int x, y;
  glfwGetWindowPos(OpenGL_Window, &x, &y);
  return (opengl_window_pos) {x, y};
}

void
OpenGL_SetWindowSize(int width, int height) {
  glfwSetWindowSize(OpenGL_Window, width, height);
}

opengl_window_size
OpenGL_GetWindowSize() {
  int w, h;
  glfwGetWindowSize(OpenGL_Window, &w, &h);
  return (opengl_window_size) {w, h};
}

bool
OpenGL_Init(int major, int minor, int windowWidth, int windowHeight, const char* windowTitle)
{

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  OpenGL_Window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, 0, 0);
  if(!OpenGL_Window) return false;
  glfwMakeContextCurrent(OpenGL_Window);
  glfwSetFramebufferSizeCallback(OpenGL_Window, OpenGL_FramebufferResizeCallback);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;
  
  glViewport(0, 0, windowWidth, windowHeight);

  return true;
}


internal void
OpenGL_DebugMessageCallback(GLenum source, GLenum type, GLuint id,
			    GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
  UNUSED(length);
  UNUSED(user_param);
  
  char* src_str = 0;
	
  switch (source)
  {
    case GL_DEBUG_SOURCE_API: src_str = "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: src_str = "WINDOW SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER: src_str = "SHADER COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY: src_str = "THIRD PARTY";
    case GL_DEBUG_SOURCE_APPLICATION: src_str = "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER: src_str = "OTHER";
  }
	
  char* type_str = 0;
	
  switch (type)
  {
    case GL_DEBUG_TYPE_ERROR: type_str =  "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str =  "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str =  "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY: type_str =  "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE: type_str =  "PERFORMANCE";
    case GL_DEBUG_TYPE_MARKER: type_str =  "MARKER";
    case GL_DEBUG_TYPE_OTHER: type_str =  "OTHER";
  }

  switch (severity)
  {
  case GL_DEBUG_SEVERITY_NOTIFICATION: DOLK_INFO("[OPENGL] [%s] [%s] %u: %s\n", src_str, type_str, id, message);
  case GL_DEBUG_SEVERITY_LOW: DOLK_INFO("[OPENGL] [%s] [%s] %u: %s\n", src_str, type_str, id, message);
  case GL_DEBUG_SEVERITY_MEDIUM: DOLK_WARNING("[OPENGL] [%s] [%s] %u: %s\n", src_str, type_str, id, message);
  case GL_DEBUG_SEVERITY_HIGH: DOLK_ERROR("[OPENGL] [%s] [%s] %u: %s\n", src_str, type_str, id, message);
  }
}

void
OpenGL_SetupDebugOutput()
{
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(OpenGL_DebugMessageCallback, 0);
}

int
main() {

  if(!OpenGL_Init(4, 6, 1280, 720, "W Goons")) return -1;
  DOLK_INFO("OpenGL Initialized!\n");
  OpenGL_SetupDebugOutput();
  OpenGL_SetClearColor(0.9f, 0.2f, 0.4f);

  glEnable(GL_DEPTH_TEST);
  
  Init();

  f64 lastFrameTime = 0;
  f64 frameTime = 0;
  f64 delta = 0;
  while(!glfwWindowShouldClose(OpenGL_Window))
  {
    frameTime = glfwGetTime();
    delta = frameTime - lastFrameTime;
    lastFrameTime = frameTime;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Update(delta);
    
    glfwSwapBuffers(OpenGL_Window);
    glfwPollEvents();    
  }
    
  return 0;
}
