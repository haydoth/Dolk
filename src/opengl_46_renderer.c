#include "renderer.h"

#include <glad/glad.h>

void OpenGL_DrawVertexArray(u32 vertexArray, u64 indicesOffset, u64 numIndices)
{
  glBindVertexArray(vertexArray);
  glDrawElements(GL_TRIANGLES, (GLsizei)numIndices, GL_UNSIGNED_INT, (void*)indicesOffset);
}
