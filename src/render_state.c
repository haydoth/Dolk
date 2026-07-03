#include "render_state.h"

Shader*
GetShader(RenderState* render, const char* name) {
  for(int i = 0; i < render->shaderCount; ++i) {
    if(strcmp(render->shaders[i].name, name) == 0) return &(render->shaders[i]);
  }
  return 0;
}

Material*
GetMaterial(RenderState* render, const char* name) {
  for(int i = 0; i < render->materialCount; ++i) {
    if(strcmp(render->materials[i].name, name) == 0) return &(render->materials[i]);
  }
  return 0;
}

Mesh*
GetMesh(RenderState* render, const char* name) {
  for(int i = 0; i < render->meshCount; ++i) {
    if(strcmp(render->meshes[i].name, name) == 0) return &(render->meshes[i]);
  }
  return 0;
}

void
LoadMesh(RenderState* render, char* path) {
  ASSERT(render->meshCount < NUM_MAX_MESHES);
  arena_temp scratch = GetScratch(0, 0);
  Mesh mesh = {0};
  
  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  obj_data data = ReadOBJBuffer(buf.Buffer, buf.Size, scratch.Arena);
  mesh.numIndices = data.Indices.Count;
  
  u64 verticesOffset = 0; 
  mesh.renderBuffer = OpenGL_CreateRenderBuffer(data.Vertices.Items, data.Vertices.Count * sizeof(f32),
					    data.Indices.Items, data.Indices.Count * sizeof(u32),
						&verticesOffset, &(mesh.indicesOffset));
  mesh.vertexArray = OpenGL_CreateVertexArray(mesh.renderBuffer, verticesOffset, data.Format);

  render->meshes[render->meshCount++] = mesh;
  ReleaseScratch(scratch); // free memory allocated by ReadX calls since it's now in the OpenGL buffer
}

void
LoadShader(RenderState* render, char* path) {
  ASSERT(render->shaderCount < NUM_MAX_SHADERS);
  arena_temp scratch = GetScratch(0, 0);
  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  Shader shader = Shader_CreateFromGLSLBuffer(buf.Buffer, buf.Size);
  render->shaders[render->shaderCount++] = shader;
  ReleaseScratch(scratch);
}

void
LoadMaterial(RenderState* render, char* path) {
  ASSERT(render->materialCount < NUM_MAX_MATERIALS);
  arena_temp scratch = GetScratch(0, 0);
  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  Material material = ReadMaterialBuffer(buf.Buffer, buf.Size);
  render->materials[render->materialCount++] = material;
  ReleaseScratch(scratch);
}
