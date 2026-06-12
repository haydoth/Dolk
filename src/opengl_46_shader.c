#include "shader.h"

#include "glad/glad.h"

typedef struct opengl_shader_source {
  da(i32) VertexStringLengths;
  da(i32) FragmentStringLengths;
  da(const char*) VertexStrings;
  da(const char*) FragmentStrings;
} opengl_shader_source;


internal opengl_shader_source
ProcessShaderString(string_view inputStr)
{
  //DOLK_INFO("Processing shader source input...\n");

  da(string_view) lines = sv_split_by_delim(inputStr, '\n', true);
  opengl_shader_source result = {0};
  
  int currentlySelected = 0;
  //DOLK_LOG("%zu\n", da_len(lines));
  //ASSERT(false);
  for(u64 lineIndex = 0; lineIndex < da_len(lines); ++lineIndex) {
    string_view line = lines[lineIndex];
    sv_trim(&line); // Necessary in order to properly check against our tokens

    //DOLK_LOG("lineIndex: %zu, mode: %i |"SV_Fmt"|\n", lineIndex, currentlySelected, SV_Arg(line));

    if(sv_cmp(line, sv("#vertex"))) {
      currentlySelected = 1; continue;
    }
    if(sv_cmp(line, sv("#fragment"))) {
      currentlySelected = 2; continue;
    }

    // Re-adds the newline removed by sv_trim
    while(lineIndex < da_len(lines) - 1 && line.Length > 0 && line.CString[line.Length - 1] != '\n')
    {
      line.Length += 1; 
    }
    
    switch(currentlySelected) {
    case 1: {
      da_append(result.VertexStrings, line.CString);
      da_append(result.VertexStringLengths, (i32)line.Length); 
    } break;
    case 2: {
      da_append(result.FragmentStrings, line.CString);
      da_append(result.FragmentStringLengths, (i32)line.Length); 
    } break;
    default: break;
    }
  }
  
  return result;
}

u32
OpenGL_CreateShaderFromGLSLBuffer(void* buffer, u64 bufferSize) {
    
  string_view file_str = {(char*)buffer, bufferSize};
  opengl_shader_source src = ProcessShaderString(file_str);

  //DOLK_INFO("Compiling shader source...\n");
  int success;
  
  u32 vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, (GLsizei)da_len(src.VertexStrings),
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
  glShaderSource(fragmentShader, (GLsizei)da_len(src.FragmentStrings),
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
