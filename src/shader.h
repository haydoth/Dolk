#pragma once

#include "sv.h"
#include "da.h"
#include "common.h"

#include <cglm/cglm.h>

u32 CreateShaderFromGLSLBuffer(void* buffer, u64 bufferSize);
void UseShader(u32 shader);

void ShaderSetUniformFloat(u32 shader, const char* name, float value);
void ShaderSetUniformVec3(u32 shader, const char* name, vec3 vector);
void ShaderSetUniformMat4(u32 shader, const char* name, mat4 matrix);
