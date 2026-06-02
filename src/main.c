
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

#define DOLK_LOG(...) printf(__VA_ARGS__);
#define DOLK_INFO(...) DOLK_LOG("[INFO] "__VA_ARGS__);
#define DOLK_WARNING(...) DOLK_LOG("[WARNING] "__VA_ARGS__);
#define DOLK_ERROR(...) DOLK_LOG("[ERROR] "__VA_ARGS__);

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

typedef struct opengl_shader_source {
  da(i32) VertexStringLengths;
  da(i32) FragmentStringLengths;
  da(const char*) VertexStrings;
  da(const char*) FragmentStrings;
} opengl_shader_source;

opengl_shader_source
ProcessShaderString(string_view inputStr)
{
  DOLK_INFO("Processing shader source input...\n");

  da(string_view) lines = sv_split_by_delim(inputStr, '\n', true);
  opengl_shader_source result = {0};
  
  int currentlySelected = 0;
  for(u64 lineIndex = 0; lineIndex < da_len(lines); lineIndex++) {
    string_view line = lines[lineIndex];
    sv_trim(&line); // Necessary in order to properly check against our tokens

    DOLK_LOG("lineIndex: %zu, mode: %i |"SV_Fmt"|\n", lineIndex, currentlySelected, SV_Arg(line));

    if(sv_equal(line, sv("#vertex"))) {
      currentlySelected = 1; continue;
    }
    if(sv_equal(line, sv("#fragment"))) {
      currentlySelected = 2; continue;
    }

    // Re-adds the newline removed by sv_trim
    while(lineIndex < da_len(lines) - 1 && line.Length > 0 && line.CString[line.Length - 1] != '\n') {
      line.Length += 1; 
    }
    
    switch(currentlySelected) {
    case 1: {
      da_append(result.VertexStrings, line.CString);
      da_append(result.VertexStringLengths, line.Length); 
    } break;
    case 2: {
      da_append(result.FragmentStrings, line.CString);
      da_append(result.FragmentStringLengths, line.Length); 
    } break;
    default: break;
    }
  }
  
  da_free(lines);
  return result;
}


u32
OpenGL_LoadShader(char* path, arena* _arena) {
  file_buffer buf = ReadEntireFile(path, _arena);
    
  if(buf.Buffer) {
    string_view file_str = sv_fb(buf);
    opengl_shader_source src = ProcessShaderString(file_str);

    DOLK_INFO("Compiling shader source...\n");
    int success;
    
    u32 vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, da_len(src.VertexStrings),
		   src.VertexStrings, src.VertexStringLengths);
    glCompileShader(vertexShader);
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
      char infoLog[512];
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      DOLK_ERROR("Failed to compile vertex shader:\n%s\n", infoLog);
    }
    
    u32 fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, da_len(src.FragmentStrings),
		   src.FragmentStrings, src.FragmentStringLengths);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
      char infoLog[512];
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
     DOLK_ERROR("Failed to compile fragment shader:\n%s\n", infoLog);
    }
    
    u32 shaderProgram;
    shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
      char infoLog[512];
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      DOLK_ERROR("Failed to link shader program:\n%s\n", infoLog);
    }    

    glUseProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    da_free(src.FragmentStrings);
    da_free(src.FragmentStringLengths);
    da_free(src.VertexStrings);
    da_free(src.VertexStringLengths);
    return shaderProgram;
  }
  return 0;
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
  case GL_DEBUG_SEVERITY_LOW: DOLK_INFO("[OPENGL] [%s] [%s] %u: %s\n", src_str, type_str, id, message);;
  case GL_DEBUG_SEVERITY_MEDIUM: DOLK_WARNING("[OPENGL] [%s] [%s] %u: %s\n", src_str, type_str, id, message);;
  case GL_DEBUG_SEVERITY_HIGH: DOLK_ERROR("[OPENGL] [%s] [%s] %u: %s\n", src_str, type_str, id, message);;
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

  arena appArena = arena_create();
  
  if(!OpenGL_Init(4, 6, 1280, 720, "W Goons")) return -1;
  DOLK_INFO("OpenGL Initialized!\n");
  OpenGL_SetupDebugOutput();
  OpenGL_SetClearColor(0.9f, 0.2f, 0.4f);
  
  float points[] = {
    0.5f,  0.5f, 0.0f,  // top right
    0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
  };
  u32 indices[] = {  // note that we start from 0!
    0, 1, 3,   // first  = 0};  
    1, 2, 3
  };

  u64 points_size_aligned = 256;
  u64 indices_size_aligned = 256;
  
  u64 points_offset = 0;
  u64 indices_offset = points_size_aligned;
  
  u32 buffer;
  glCreateBuffers(1, &buffer);
  glNamedBufferStorage(buffer, points_size_aligned + indices_size_aligned, 0, GL_DYNAMIC_STORAGE_BIT);

  glNamedBufferSubData(buffer, points_offset, sizeof(points), points);
  glNamedBufferSubData(buffer, indices_offset, sizeof(indices), indices);
  
  u32 vertexArray;
  glCreateVertexArrays(1, &vertexArray);
  glVertexArrayVertexBuffer(vertexArray, 0, buffer, points_offset, sizeof(float)*3);
  glVertexArrayElementBuffer(vertexArray, buffer);
  
  glEnableVertexArrayAttrib(vertexArray, 0);
  glVertexArrayAttribFormat(vertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vertexArray, 0, 0);
  
  glBindVertexArray(vertexArray);
  u32 testShader = OpenGL_LoadShader("../shaders/test.glsl", &appArena);
  
  while(!glfwWindowShouldClose(OpenGL_Window))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(testShader);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)indices_offset);
    
    glfwSwapBuffers(OpenGL_Window);
    glfwPollEvents();    
  }
    
  return 0;
}
