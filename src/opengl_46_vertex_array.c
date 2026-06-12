#include "vertex_array.h"

#include "glad/glad.h"

// NOTE: current implementation only supports float values
// NOTE: format is a dynamic array of float counts

u32
OpenGL_CreateVertexArray(u32 buffer, u64 verticesOffset, da(u32) format) {
  
  u32 vertexArray;
  glCreateVertexArrays(1, &vertexArray);
  glVertexArrayVertexBuffer(vertexArray, 0, buffer, verticesOffset, sizeof(float)*8);
  glVertexArrayElementBuffer(vertexArray, buffer);

  u32 currentOffset = 0;
  u32 currentByteSize = 0;
  for(u32 i = 0; i < da_len(format); ++i) {

    currentByteSize = format[i] * sizeof(float);
    
    glEnableVertexArrayAttrib(vertexArray, i);
    glVertexArrayAttribFormat(vertexArray, i, format[i],
			      GL_FLOAT, GL_FALSE, currentOffset);
    glVertexArrayAttribBinding(vertexArray, i, 0); // we only have 1 buffer so bind to 0

    DOLK_LOG("float count: %lu, offset: %lu\n", format[i], currentOffset);
    currentOffset += currentByteSize;
  }
  return vertexArray;
}

void
OpenGL_BindVertexArray(u32 vertexArray) {

  glBindVertexArray(vertexArray);
}
