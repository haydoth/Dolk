// Dolk
#include "dolk.h"

#include "common.h"
#include "window.h"
#include "input.h"

// STD
#include <stdio.h>
#include <string.h>
#include <math.h>

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

  AppState app = {0};
  if(!CreateWindow(&(app.window), "Dolk", 1280, 720)) return -1;
  OpenGL_SetupDebugOutput();

  glfwWindowHint(GLFW_SAMPLES, 4);
  glEnable(GL_MULTISAMPLE);    
  glEnable(GL_DEPTH_TEST);
  
  Init(&app);

  f64 lastFrameTime = 0;
  f64 frameTime = 0;
  f64 delta = 0;
  while(!glfwWindowShouldClose((GLFWwindow*)app.window.handle))
  {
    frameTime = glfwGetTime();
    delta = frameTime - lastFrameTime;
    lastFrameTime = frameTime;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Update(&app, delta);
    Input_EndFrame(&(app.window.input));
    
    glfwSwapBuffers((GLFWwindow*)app.window.handle);
    glfwPollEvents();    
  }
    
  return 0;
}
