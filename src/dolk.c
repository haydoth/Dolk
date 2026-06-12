
#include "dolk.h"

#include "common.h"
#include "audio.h"
#include "arena.h"
#include "file_io.h"
#include "wav.h"
#include "obj.h"
#include "buffer.h"
#include "vertex_array.h"
#include "renderer.h"
#include "shader.h"

u32
LoadWAV(char* path, arena* _arena) {
  arena_temp temp = arena_temp_begin(_arena);

  file_buffer buf = ReadEntireFile(path, _arena);
  wav_data data = ReadWAVBuffer(buf.Buffer, buf.Size, _arena);
  u32 audioBuffer = Audio_CreateBufferWAV(data.PCMData, data.PCMDataSize,
					  data.SampleRate, data.Format,
					  data.NumChannels, data.BitsPerSample);

  arena_temp_end(temp); // free memory allocated by ReadX calls since it's now in the OpenAL buffer
  return audioBuffer;
}

u32
LoadOBJ(char* path, arena* _arena, u32* renderBuffer, u64* indicesOffset, u64* numIndices) {

  arena_temp temp = arena_temp_begin(_arena);

  file_buffer buf = ReadEntireFile(path, _arena);
  obj_data data = ReadOBJBuffer(buf.Buffer, buf.Size);
  *numIndices = da_len(data.Indices);
  
  u64 verticesOffset = 0; 
  *renderBuffer = OpenGL_CreateRenderBuffer(data.Vertices, da_len(data.Vertices) * sizeof(f32),
					    data.Indices, da_len(data.Indices) * sizeof(u32),
					    &verticesOffset, indicesOffset);
  u32 vertexArray = OpenGL_CreateVertexArray(*renderBuffer, verticesOffset, data.Format);

  arena_temp_end(temp); // free memory allocated by ReadX calls since it's now in the OpenGL buffer
  return vertexArray;
}

u32
LoadGLSL(char* path, arena* _arena) {

  arena_temp temp = arena_temp_begin(_arena);

  file_buffer buf = ReadEntireFile(path, _arena);
  u32 shader = OpenGL_CreateShaderFromGLSLBuffer(buf.Buffer, buf.Size);

  arena_temp_end(temp); // free memory allocated by ReadX calls since it's now in the OpenGL buffer
  return shader;
  
}

global u32 vertexArray, renderBuffer, testShader;
global u64 indicesOffset, numIndices;
global mat4 model, view, proj;

void
Init() {
  Audio_Init();

  arena myArena = arena_create();
  u32 audioBuffer = LoadWAV("../res/music.wav", &myArena);
  u32 audioSource = Audio_CreateSource(audioBuffer);

  Audio_SourceSetLooping(audioSource, true);
  Audio_SourceSetGain(audioSource, 0.05f);
  Audio_SourceSetPitch(audioSource, 1.0f);
  Audio_PlaySource(audioSource);

  vertexArray = LoadOBJ("../res/lucy.obj", &myArena, &renderBuffer, &indicesOffset, &numIndices);
  OpenGL_BindVertexArray(vertexArray);
  testShader = LoadGLSL("../res/test.glsl", &myArena);

  vec3 pos = {0, 0, 0}, lookDir = {0, 0, -1}, upDir = {0, 1, 0};
  glm_mat4_identity(model);
  glm_mat4_identity(view);
  glm_mat4_identity(proj);
  glm_translate(model, (vec3) {0, 0, -3});
  glm_look(pos, lookDir, upDir, view);
  glm_perspective(glm_rad(75.0f), 16.0f / 9.0f, 0.1f, 3000.0f, proj);
  glm_mat4_mul(proj, view, view);
  OpenGL_UseShader(testShader);
  OpenGL_ShaderSetUniformMat4(testShader, "MODEL_MATRIX", model);
  OpenGL_ShaderSetUniformMat4(testShader, "VIEW_MATRIX", view);
}

void
Update(f64 delta)
{
  glm_rotate_y(model, glm_rad(8.0f * (f32)delta), model);
  
  OpenGL_UseShader(testShader);
  OpenGL_ShaderSetUniformMat4(testShader, "MODEL_MATRIX", model);
  OpenGL_BindIndexBuffer(renderBuffer);
  OpenGL_DrawVertexArray(vertexArray, indicesOffset, numIndices);
}

