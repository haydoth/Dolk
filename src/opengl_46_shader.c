#include "shader.h"

#include "glad/glad.h"

typedef struct opengl_shader_source {
  CStrings VertexStrings;
  CStrings FragmentStrings;
  I32s VertexStringLengths;
  I32s FragmentStringLengths;
} opengl_shader_source;

internal opengl_shader_source
ProcessShaderString(string_view inputStr, arena* _arena)
{
  opengl_shader_source result = {0};  
  int currentlySelected = 0;
  
  while(inputStr.Length > 0) {
    string_view line = sv_split(&inputStr, '\n', true);
    string_view copy = line;
    sv_trim(&copy); 

    if(sv_cmp(copy, sv("#vertex"))) {
      currentlySelected = 1;
      continue;
    }
    if(sv_cmp(copy, sv("#fragment"))) {
      currentlySelected = 2;
      continue;
    }

    switch(currentlySelected) {
    case 1: {
      *da_append(result.VertexStrings, _arena) = line.CString;
      *da_append(result.VertexStringLengths, _arena) = (i32)line.Length; 
    } break;
    case 2: {
      *da_append(result.FragmentStrings, _arena) = line.CString;
      *da_append(result.FragmentStringLengths, _arena) = (i32)line.Length; 
    } break;
    default: break;
    }
  }
  
  return result;
}

u32
OpenGL_CreateShaderFromGLSLBuffer(void* buffer, u64 bufferSize, arena* _arena) {
    
  string_view file_str = {(char*)buffer, bufferSize};
  opengl_shader_source src = ProcessShaderString(file_str, _arena);

  int success;
  
  u32 vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, (GLsizei)src.VertexStrings.Count, src.VertexStrings.Items, src.VertexStringLengths.Items);
  glCompileShader(vertexShader);
  
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if(!success) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    DOLK_ERROR("Failed to compile vertex shader:\n%s\n", infoLog);
  }
  
  u32 fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, (GLsizei)src.FragmentStrings.Count, src.FragmentStrings.Items, src.FragmentStringLengths.Items);
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

  return shaderProgram;
}

void
OpenGL_UseShader(u32 shader) {
  glUseProgram(shader);
}

void
OpenGL_ShaderSetUniformMat4(u32 shader, const char* name, mat4 matrix) {
  GLuint location = glGetUniformLocation(shader, name);
  glUniformMatrix4fv(location, 1, GL_FALSE, matrix[0]);
}
