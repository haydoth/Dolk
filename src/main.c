
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

#define CH_LOG(...) printf(__VA_ARGS__);
#define CH_INFO(...) CH_LOG("[INFO] "__VA_ARGS__);
#define CH_WARNING(...) CH_LOG("[WARNING] "__VA_ARGS__);
#define CH_ERROR(...) CH_LOG("[ERROR] "__VA_ARGS__);

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
  da(u32) VertexStringLengths;
  da(u32) FragmentStringLengths;
  da(const char*) VertexStrings;
  da(const char*) FragmentStrings;
} opengl_shader_source;

opengl_shader_source
ProcessShaderString(string_view inputStr)
{
  CH_INFO("Processing shader source input...\n");

  da(string_view) lines = sv_split_by_delim(inputStr, '\n', true);
  opengl_shader_source result = {0};
  
  int currentlySelected = 0;
  for(u64 lineIndex = 0; lineIndex < da_len(lines); lineIndex++) {
    string_view line = lines[lineIndex];
    sv_trim(&line); // Necessary in order to properly check against our tokens

    CH_LOG("lineIndex: %zu, mode: %i |"SV_Fmt"|\n", lineIndex, currentlySelected, SV_Arg(line));

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

    CH_INFO("Compiling shader source...\n");
    int success;
    
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, da_len(src.VertexStrings),
		   src.VertexStrings, src.VertexStringLengths);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
      char infoLog[512];
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      CH_ERROR("Failed to compile vertex shader:\n%s\n", infoLog);
    }
    
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, da_len(src.FragmentStrings),
		   src.FragmentStrings, src.FragmentStringLengths);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
      char infoLog[512];
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      CH_ERROR("Failed to compile fragment shader:\n%s\n", infoLog);
    }
    
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
      char infoLog[512];
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      CH_ERROR("Failed to link shader program:\n%s\n", infoLog);
    }    

    glUseProgram(shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    return shaderProgram;
  }
  return 0;
}

typedef struct opengl_vertex_array {

  u32 Handle;
  u32 IndexBuffer;
  da(u32) VertexBuffers;
  
} opengl_vertex_array;

opengl_vertex_array
OpenGL_CreateVertexArray() {
  
}

int
main() {

  arena appArena = arena_create();
  
  if(!OpenGL_Init(4, 6, 1280, 720, "W Goons")) return -1;
  CH_INFO("OpenGL Initialized!\n");
  OpenGL_SetClearColor(0.9f, 0.2f, 0.4f);
  
  float points[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f};

  u32 vertexArrays[1];
  glCreateVertexArrays(1, &vertexArrays[0]);

  u32 vertexBuffers[1];
  glCreateBuffers(1, &vertexBuffers[0]);
  glNamedBufferStorage(vertexBuffers[0], sizeof(points), points, GL_DYNAMIC_STORAGE_BIT);

  glVertexArrayVertexBuffer(vertexArrays[0], 0, vertexBuffers[0], 0, sizeof(float)*3);
  glEnableVertexArrayAttrib(vertexArrays[0], 0);
  glVertexArrayAttribFormat(vertexArrays[0], 0, 3, GL_FLOAT, GL_FALSE, 0);
  glVertexArrayAttribBinding(vertexArrays[0], 0, 0);
  
  u32 testShader = OpenGL_LoadShader("../shaders/test.glsl", &appArena);
  
  while(!glfwWindowShouldClose(OpenGL_Window))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(testShader);
    glBindVertexArray(vertexArrays[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glfwSwapBuffers(OpenGL_Window);
    glfwPollEvents();    
  }
    
  return 0;
}
