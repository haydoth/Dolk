#include "dolk.h"

#include "common.h"
#include "arena.h"
#include "loaders.h"
#include "render_state.h"

void
SetColorScheme(u32 shader, vec3 light, vec3 bg, float ambientStrength) {
  SetClearColorV(bg);
  Shader_Use(shader);
  Shader_SetUniformFloat(shader, "AMBIENT_STRENGTH", ambientStrength);
  Shader_SetUniformVec3(shader, "AMBIENT_COLOR", bg);
  Shader_SetUniformVec3(shader, "LIGHT_COLOR", light);
}

global u32 testShader;
global mat4 view, proj;

void
Init(AppState *app) {
  Audio_Init();

  u32 audioBuffer = LoadWAVSound("../res/music.wav");
  u32 audioSource = Audio_CreateSource(audioBuffer);

  Audio_SourceSetLooping(audioSource, true);
  Audio_SourceSetGain(audioSource, 0.05f);
  Audio_SourceSetPitch(audioSource, 1.0f);
  //Audio_PlaySource(audioSource);

  RenderState *render = &(app->render);
  LoadMesh(render, "../res/lucy.obj");
  LoadShader(render, "../res/test.glsl");
  LoadMaterial(render, "../res/test.mat");
  render->subject.mesh = GetMesh("lucy");
  render->subject.material = GetMaterial("test");
  
  glm_mat4_identity(render->subject.transform);
  glm_mat4_identity(render->view);
  glm_mat4_identity(render->projection);
  
  glm_translate(render->subject.transform, (vec3) {0, -80, -150});
  glm_look(render->eye.position, render->eye.forward, render->up, render->view);
  glm_perspective(glm_rad(75.0f), 16.0f / 9.0f, 0.1f, 3000.0f, render->projection);

  vec3 light = {0.0f, 0.67f, 1.0f}, bg = {1.0f, 0.0f, 0.5f};
  SetColorScheme(testShader, light, bg, 0.2f);  

  render->up[1] = 1;
  render->eye.forward[2] = -1;

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
  
  int in_x = KeyIsPressed(input, 'D') - KeyIsPressed(input, 'A');
  int in_y = KeyIsPressed(input, DOLK_KEY_SPACE) - KeyIsPressed(input, DOLK_KEY_LEFT_CONTROL);
  int in_z = KeyIsPressed(input, 'S') - KeyIsPressed(input, 'W');

  vec3 move = {(f32)in_x, (f32)in_y, (f32)in_z};
  glm_vec3_scale(move, app->game.playerSpeed * (f32)delta, move);
  glm_vec3_add(render->eye.position, move, render->eye.position);
  
  glm_look(render->eye.position, render->eye.forward, render->up, render->view);
  glm_rotate_y(render->subject->transform, glm_rad(16.0f * (f32)delta), render->subject->transform);

  DrawRenderable(render->subject);
}

