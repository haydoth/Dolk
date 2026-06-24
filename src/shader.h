#pragma once

#include "sv.h"
#include "da.h"
#include "common.h"

#include <cglm/cglm.h>

u32 Shader_CreateFromGLSLBuffer(void* buffer, u64 bufferSize);
void Shader_Use(u32 shader);

void Shader_SetUniformFloat(u32 shader, const char* name, float value);
void Shader_SetUniformVec3(u32 shader, const char* name, vec3 vector);
void Shader_SetUniformMat4(u32 shader, const char* name, mat4 matrix);
