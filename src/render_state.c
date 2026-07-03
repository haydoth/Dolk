#include "render_state.h"
#include "common.h"
#include "shader.h"
#include "obj.h"
#include "file_io.h"
#include "vertex_array.h"
#include "buffer.h"
#include "material.h"

RenderableObject*
GetRenderable(RenderState* render, const char* name) {
  for(int i = 0; i < render->renderableCount; ++i) {
    if(strcmp(render->renderables[i].name, name) == 0) return render->renderables + i;
  }
  ASSERT(render->renderableCount < NUM_MAX_RENDERABLES);
  render->renderables[render->renderableCount].name = name; 
  return render->renderables + render->renderableCount++;
  
}

Shader*
GetShader(RenderState* render, const char* name) {
  for(int i = 0; i < render->shaderCount; ++i) {
    if(strcmp(render->shaders[i].name, name) == 0) return render->shaders + i;
  }
  return 0;
}

Material*
GetMaterial(RenderState* render, const char* name) {
  for(int i = 0; i < render->materialCount; ++i) {
    if(strcmp(render->materials[i].name, name) == 0) return render->materials + i;
  }
  return 0;
}

Mesh*
GetMesh(RenderState* render, const char* name) {
  for(int i = 0; i < render->meshCount; ++i) {
    if(strcmp(render->meshes[i].name, name) == 0) return render->meshes + i;
  }
  return 0;
}
Mesh*
LoadMesh(RenderState* render, char* path, const char* name) {
  ASSERT(render->meshCount < NUM_MAX_MESHES);
  arena_temp scratch = GetScratch(0, 0);
  Mesh mesh = {0};
  mesh.name = name;
  
  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  obj_data data = ReadOBJBuffer(buf.Buffer, buf.Size, scratch.Arena);
  mesh.numIndices = data.Indices.Count;
  
  u64 verticesOffset = 0; 
  mesh.renderBuffer = OpenGL_CreateRenderBuffer(
     data.Vertices.Items, data.Vertices.Count * sizeof(f32),
     data.Indices.Items, data.Indices.Count * sizeof(u32),
     &verticesOffset, &(mesh.indicesOffset));
  mesh.vertexArray = OpenGL_CreateVertexArray(
					      mesh.renderBuffer,
					      verticesOffset,
					      data.Format);

  render->meshes[render->meshCount++] = mesh;
  ReleaseScratch(scratch); // free memory allocated by ReadX calls since it's now in the OpenGL buffer
  return render->meshes + render->meshCount - 1;
}

Shader*
LoadShader(RenderState* render, char* path, const char* name) {
  ASSERT(render->shaderCount < NUM_MAX_SHADERS);
  arena_temp scratch = GetScratch(0, 0);
  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  u32 program = Shader_CreateProgramFromGLSLBuffer(buf.Buffer, buf.Size);
  ReleaseScratch(scratch);

  Shader* alreadyExisting = GetShader(render, name);
  if(alreadyExisting) {
    alreadyExisting->handle = program;
    return alreadyExisting;
  }

  render->shaders[render->shaderCount++] = (Shader){name, program};
  return render->shaders + render->shaderCount - 1;
}

Material*
LoadMaterial(RenderState* render, arena* persistent, char* path, const char* name, Shader* shader) {
  ASSERT(render->materialCount < NUM_MAX_MATERIALS);
  arena_temp scratch = GetScratch(0, 0);
  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  Material material = ReadMaterialBuffer(buf.Buffer, buf.Size, persistent);
  material.name = name;
  material.shader = shader;

  Material* alreadyExisting = GetMaterial(render, name);
  if(alreadyExisting) {
    *alreadyExisting = material;
    return alreadyExisting;
  }
  
  render->materials[render->materialCount++] = material;
  ReleaseScratch(scratch);
  return render->materials + render->materialCount - 1;
}
