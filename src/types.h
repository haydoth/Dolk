#pragma once

#include "common.h"
#include "input_codes.h"

#include <cglm/cglm.h>

typedef struct {
  const char* name;
  unsigned int handle; // if the handle is 0 then the shader isn't loaded
} Shader;

typedef enum {
  MAT_PROP_FLOAT, MAT_PROP_VEC3, MAT_PROP_TEXTURE2D
} MaterialPropertyKind;

typedef struct {

  union {
    float Float;
    vec3 Vector3;
    u32 Texture2D;
  } as;
  
  const char* name;
  MaterialPropertyKind kind;

} MaterialProperty;

typedef struct {

  MaterialProperty properties[32];
  const char* name;
  const char* shaderName;
  Shader* shader;
  u8 propertyCount;
  
} Material;

typedef struct {

  const char* name;
  u64 indicesOffset, numIndices;
  u32 vertexArray, renderBuffer;
  
} Mesh;

typedef struct {

  mat4 transform; 
  Material* material;
  Mesh* mesh;

} RenderableObject;

#define NUM_MAX_MESHES 32
#define NUM_MAX_SHADERS 16
#define NUM_MAX_MATERIALS 16

typedef struct {

  Mesh meshes[NUM_MAX_MESHES];
  Shader shaders[NUM_MAX_SHADERS];
  Material materials[NUM_MAX_MATERIALS];
  
  int shaderCount, materialCount, meshCount;
  
  RenderableObject subject;
  mat4 view;
  mat4 projection;
  
  struct {
    vec3 position;
    vec3 forward;
  } eye;

  vec3 up;
  
} RenderState;

#define MAX_AUDIO_BUFFERS 64
// will probably live on a separate thread  
typedef struct {
  u32 buffers[MAX_AUDIO_BUFFERS];
} AudioState;

typedef struct {

  char currentKeyStates[DOLK_KEY_COUNT];
  char lastKeyStates[DOLK_KEY_COUNT]; 
  float currentMousePosition[2];
  float lastMousePosition[2];
  
} InputState;

typedef struct {

  InputState input;
  void* handle;
  
} WindowState;
