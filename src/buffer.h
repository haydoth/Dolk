#pragma once

#include "common.h"

// NOTE: Because we put vertices and indices in the same buffer, the vertex array creation
// needs the offset into the buffer where vertices are stored, which is probably
// always gonna be zero. However, the draw command obviously also needs the offset into the
// buffer where the indices are stored, which is not as trivial

u32 OpenGL_CreateRenderBuffer(f32* vertices, u64 verticesSize,
			      u32* indices, u64 indicesSize,
			      u64* verticesOffset, u64* indicesOffset);


void OpenGL_BindIndexBuffer(u32 buffer);
