
#include "dolk.h"

#include "common.h"
#include "arena.h"
#include "file_io.h"
#include "wav.h"
#include "obj.h"
#include "buffer.h"
#include "vertex_array.h"
#include "shader.h"

u32
LoadWAVSound(char* path) {
  arena_temp scratch = GetScratch(0, 0);

  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  wav_data data = ReadWAVBuffer(buf.Buffer, buf.Size, scratch.Arena);
  u32 audioBuffer = Audio_CreateBufferWAV(data.PCMData, data.PCMDataSize,
					  data.SampleRate, data.Format,
					  data.NumChannels, data.BitsPerSample);
  ReleaseScratch(scratch); 
  return audioBuffer;
}

u32
LoadOBJMesh(char* path, u32* renderBuffer, u64* indicesOffset, u64* numIndices) {

  arena_temp scratch = GetScratch(0, 0);

  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  obj_data data = ReadOBJBuffer(buf.Buffer, buf.Size, scratch.Arena);
  *numIndices = data.Indices.Count;
  
  u64 verticesOffset = 0; 
  *renderBuffer = OpenGL_CreateRenderBuffer(data.Vertices.Items, data.Vertices.Count * sizeof(f32),
					    data.Indices.Items, data.Indices.Count * sizeof(u32),
					    &verticesOffset, indicesOffset);
  u32 vertexArray = OpenGL_CreateVertexArray(*renderBuffer, verticesOffset, data.Format);

  ReleaseScratch(scratch); // free memory allocated by ReadX calls since it's now in the OpenGL buffer
  return vertexArray;
}

u32
LoadGLSLShader(char* path) {

  arena_temp scratch = GetScratch(0, 0);

  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  u32 shader = Shader_CreateFromGLSLBuffer(buf.Buffer, buf.Size);

  ReleaseScratch(scratch);
  return shader;
}

void
SetColorScheme(u32 shader, vec3 light, vec3 bg, float ambientStrength) {
  SetClearColorV(bg);
  Shader_Use(shader);
  Shader_SetUniformFloat(shader, "AMBIENT_STRENGTH", ambientStrength);
  Shader_SetUniformVec3(shader, "AMBIENT_COLOR", bg);
  Shader_SetUniformVec3(shader, "LIGHT_COLOR", light);
}

global u32 vertexArray, renderBuffer, testShader;
global u64 indicesOffset, numIndices;
global mat4 model, view, proj;

void
Init(AppState *app) {
  Audio_Init();

  u32 audioBuffer = LoadWAVSound("../res/music.wav");
  u32 audioSource = Audio_CreateSource(audioBuffer);

  Audio_SourceSetLooping(audioSource, true);
  Audio_SourceSetGain(audioSource, 0.05f);
  Audio_SourceSetPitch(audioSource, 1.0f);
  //Audio_PlaySource(audioSource);

  vertexArray = LoadOBJMesh("../res/lucy.obj", &renderBuffer, &indicesOffset, &numIndices);
  OpenGL_BindVertexArray(vertexArray);
  testShader = LoadGLSLShader("../res/test.glsl");

  glm_mat4_identity(model);
  glm_mat4_identity(view);
  glm_mat4_identity(proj);
  
  glm_translate(model, (vec3) {0, -80, 150});
  glm_look(app->render.eye.position, app->render.eye.forward, app->render.up, view);
  glm_perspective(glm_rad(75.0f), 16.0f / 9.0f, 0.1f, 3000.0f, proj);

  vec3 light = {0.0f, 0.67f, 1.0f}, bg = {1.0f, 0.0f, 0.5f};
  SetColorScheme(testShader, light, bg, 0.2f);  

  app->render.up[1] = 1;
  app->render.eye.forward[2] = 1;
  app->game.playerSpeed = 20.0f;
}

void
Update(AppState *app, f64 delta)
{
  RenderState* render = &(app->render);
  InputState* input = &(app->window.input);

  if(KeyWasJustReleased(input, 'R')) {
    testShader = LoadGLSLShader("../res/test.glsl");
    vec3 light = {0.0f, 0.67f, 1.0f}, bg = {1.0f, 0.0f, 0.5f};
    SetColorScheme(testShader, light, bg, 0.2f);
  }
  
  int in_x = KeyIsPressed(input, 'A') - KeyIsPressed(input, 'D');
  int in_y = KeyIsPressed(input, ' ') - KeyIsPressed(input, 'C');
  int in_z = KeyIsPressed(input, 'W') - KeyIsPressed(input, 'S');

  vec3 move = {(f32)in_x, (f32)in_y, (f32)in_z};
  glm_vec3_scale(move, app->game.playerSpeed * (f32)delta, move);
  glm_vec3_add(render->eye.position, move, render->eye.position);
  glm_look(render->eye.position, render->eye.forward, render->up, view);
  
  glm_rotate_y(model, glm_rad(16.0f * (f32)delta), model);
  
  Shader_Use(testShader);
  Shader_SetUniformFloat(testShader, "TIME", (f32)GetElapsedTime());
  Shader_SetUniformMat4(testShader, "MODEL_MATRIX", model);

  mat4 view_proj;
  glm_mat4_mul(proj, view, view_proj);

  Shader_SetUniformMat4(testShader, "VIEW_MATRIX", view_proj);
  OpenGL_BindIndexBuffer(renderBuffer);
  OpenGL_DrawVertexArray(vertexArray, indicesOffset, numIndices);
}

