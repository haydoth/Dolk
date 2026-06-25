#pragma once

#include <cglm/cglm.h>

unsigned int Shader_CreateFromGLSLBuffer(void* buffer, unsigned long long bufferSize);
void Shader_Use(unsigned int shader);

void Shader_SetUniformFloat(unsigned int shader, const char* name, float value);
void Shader_SetUniformVec3(unsigned int shader, const char* name, vec3 vector);
void Shader_SetUniformMat4(unsigned int shader, const char* name, mat4 matrix);
