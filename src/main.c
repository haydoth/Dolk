
// STD
#include <stdio.h>
#include <string.h>
#include <math.h>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Chisel
#include "common.h"
#include "arena.h"
#include "file_io.h"
#include "sv.h"

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

  TODO:
  - Platform layer
    - Read/write files [CHECK]
    - Play sounds
    - Fullscreen support
  - Game layer
    - Should never interact with OpenGL layer

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

string_view
sv_fb(file_buffer fb)
{
  return (string_view) {fb.Buffer, fb.Size};
}

void
ProcessShaderString(string_view inputStr)
{
  UNUSED(inputStr);
}

int
main() {

  arena appArena = arena_create();
  
  if(!OpenGL_Init(4, 6, 1280, 720, "W Goons")) return -1;
  OpenGL_SetClearColor(0.9f, 0.2f, 0.4f);
  
  float points[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f};

  u32 vertexBufferObject;
  glCreateBuffers(1, &vertexBufferObject);
  glNamedBufferData(vertexBufferObject, sizeof(points), &points, GL_STATIC_DRAW);

  file_buffer buf = ReadEntireFile("shaders/test.glsl", &appArena);

  if(buf.Buffer) {
    string_view file_str = sv_fb(buf);
    da(string_view) lines = split_by_delim(file_str, '\n');

    for(u64 i = 0; i < da_len(lines); i++) {
      printf(SV_Fmt"\n", SV_Arg(lines[i]));
    }
  }
  
  
  while(!glfwWindowShouldClose(OpenGL_Window))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    glfwSwapBuffers(OpenGL_Window);
    glfwPollEvents();    
  }
    
  return 0;
}
