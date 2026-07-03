#include "shader.h"

#include "sv.h"
#include "da.h"
#include "common.h"

#include "glad/glad.h"

typedef struct opengl_shader_source {
  CStrings VertexStrings;
  CStrings FragmentStrings;
  I32s VertexStringLengths;
  I32s FragmentStringLengths;
  string_view Name;
} opengl_shader_source;

internal opengl_shader_source
ProcessShaderString(string_view inputStr)
{
  opengl_shader_source result = {0};  
  int currentlySelected = 0;

  arena_temp scratch = GetScratch(0, 0);
  while(inputStr.Length > 0) {
    string_view line = sv_split(&inputStr, '\n', true);
    string_view copy = line;
    sv_trim(&copy); 

    if(sv_word_count(copy) > 1) {
      string_view token = sv_split(&copy, ' ', false);
      if(sv_cmp(copy, sv("#shader"))) {
	result.Name = copy;
	continue;
      } 
    }
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
      *da_append(result.VertexStrings, scratch.Arena) = line.CString;
      *da_append(result.VertexStringLengths, scratch.Arena) = (i32)line.Length; 
    } break;
    case 2: {
      *da_append(result.FragmentStrings, scratch.Arena) = line.CString;
      *da_append(result.FragmentStringLengths, scratch.Arena) = (i32)line.Length; 
    } break;
    default: break;
    }
  }
  
  return result;
}

internal u32
CompileShaderSource(opengl_shader_source src) {

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
    return 0;
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
   return 0;
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
    return 0;
  }    

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  
  return shaderProgram;
}

Shader
Shader_CreateFromGLSLBuffer(void* buffer, unsigned long long bufferSize) {
    
  string_view file_str = {(char*)buffer, bufferSize};
  opengl_shader_source src = ProcessShaderString(file_str);
  unsigned int shaderProgram = CompileShaderSource(src);

  return (Shader) {src.Name, shaderProgram};
}

void
Shader_Use(unsigned int shader) {
  glUseProgram(shader);
}

void
Shader_SetUniformFloat(unsigned int shader, const char* name, float value) {
  GLuint location = glGetUniformLocation(shader, name);
  glUniform1f(location, value);
}

void
Shader_SetUniformVec3(unsigned int shader, const char* name, vec3 vector) {
  GLuint location = glGetUniformLocation(shader, name);
  glUniform3fv(location, 1, vector);
}

void
Shader_SetUniformMat4(unsigned int shader, const char* name, mat4 matrix) {
  GLuint location = glGetUniformLocation(shader, name);
  glUniformMatrix4fv(location, 1, GL_FALSE, matrix[0]);
}
