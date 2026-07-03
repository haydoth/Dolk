#include "renderer.h"

#include "types.h"
#include "material.h"
#include "shader.h"
#include "window.h"
#include "vertex_array.h"
#include "buffer.h"

#include <cglm/cglm.h>
#include <glad/glad.h>

void DrawRenderable(RenderState* render, RenderableObject renderable)
{
  Mesh* mesh = renderable.mesh;
  Material* mat = renderable.material;
  ASSERT(mesh && mat);

  BindMaterial(mat);
  
  Shader_SetUniformFloat(mat->shader->handle, "TIME", (f32)GetElapsedTime());
  Shader_SetUniformMat4(mat->shader->handle, "MODEL_MATRIX", renderable.transform);
  Shader_SetUniformMat4(mat->shader->handle, "VIEW_MATRIX", render->view);
  
  mat4 view_proj;
  glm_mat4_mul(render->projection, render->view, view_proj);

  Shader_SetUniformMat4(mat->shader->handle, "VIEW_PROJECTION_MATRIX", view_proj);
  OpenGL_BindIndexBuffer(mesh->renderBuffer);

  OpenGL_BindVertexArray(mesh->vertexArray);
  glDrawElements(GL_TRIANGLES, (GLsizei)(mesh->numIndices), GL_UNSIGNED_INT, (void*)(mesh->indicesOffset));
}
