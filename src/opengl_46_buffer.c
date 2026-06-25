#include "buffer.h"

#include "glad/glad.h"

internal u64
AlignForwardToUBOA(u64 size) {
  i32 alignment;
  glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

  u64 modulo = size % alignment;
  if(modulo != 0) {
    return size + alignment - modulo; 
  }
  return size;
}

u32
OpenGL_CreateRenderBuffer(f32* vertices, u64 verticesSize, u32* indices, u64 indicesSize, u64* verticesOffset, u64* indicesOffset)
{
  ASSERT(vertices && verticesSize && indices && indicesSize);
  u64 verticesSizeAligned = AlignForwardToUBOA(verticesSize);
  u64 indicesSizeAligned = AlignForwardToUBOA(indicesSize);

  *verticesOffset = 0;
  *indicesOffset = verticesSizeAligned;
  
  u32 buffer;
  glCreateBuffers(1, &buffer);
  glNamedBufferStorage(buffer, verticesSizeAligned + indicesSizeAligned,
		       0, GL_DYNAMIC_STORAGE_BIT);

  glNamedBufferSubData(buffer, *verticesOffset, verticesSize, vertices);
  glNamedBufferSubData(buffer, *indicesOffset, indicesSize, indices);

  return buffer;
}

void
OpenGL_BindIndexBuffer(u32 buffer) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}
