#pragma once

#include "common.h"
#include "da.h"

u32 OpenGL_CreateVertexArray(u32 buffer, u64 verticesOffset, da(u32) format);
void OpenGL_BindVertexArray(u32 vertexArray);
