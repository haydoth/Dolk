#pragma once

#include "common.h"
#include "da.h"

u32 OpenGL_CreateVertexArray(u32 buffer, u64 verticesOffset, U32s format);
void OpenGL_BindVertexArray(u32 vertexArray);
