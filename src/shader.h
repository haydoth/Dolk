#pragma once

#include "sv.h"
#include "da.h"
#include "common.h"

#include <cglm/cglm.h>

u32 OpenGL_CreateShaderFromGLSLBuffer(void* buffer, u64 bufferSize);
void OpenGL_UseShader(u32 shader);

void OpenGL_ShaderSetUniformMat4(u32 shader, const char* name, mat4 matrix);
