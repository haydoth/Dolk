#include "vertex_array.h"

#include "glad/glad.h"

// NOTE: current implementation only supports float values
// NOTE: format is a dynamic array of float counts

u32
OpenGL_CreateVertexArray(u32 buffer, u64 verticesOffset, U32s format) {
  
  u32 vertexArray;
  glCreateVertexArrays(1, &vertexArray);
  u32 floatCount = 0;
  for(u64 i = 0; i < format.Count; ++i) floatCount += format.Items[i];
  glVertexArrayVertexBuffer(vertexArray, 0, buffer, verticesOffset, sizeof(float)*floatCount);
  glVertexArrayElementBuffer(vertexArray, buffer);

  u32 currentOffset = 0;
  u32 currentByteSize = 0;
  for(u32 i = 0; i < format.Count; ++i) {

    currentByteSize = format.Items[i] * sizeof(float);
    
    glEnableVertexArrayAttrib(vertexArray, i);
    glVertexArrayAttribFormat(vertexArray, i, format.Items[i], GL_FLOAT, GL_FALSE, currentOffset);
    glVertexArrayAttribBinding(vertexArray, i, 0); // we only have 1 buffer so bind to 0

    currentOffset += currentByteSize;
  }
  return vertexArray;
}

void
OpenGL_BindVertexArray(u32 vertexArray) {

  glBindVertexArray(vertexArray);
}
