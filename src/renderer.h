#pragma once

#include "common.h"

#include <cglm/types.h>

typedef struct {

  struct {
    vec3 position;
    vec3 forward;
  } eye;

  vec3 up;
  
} RenderState;

void OpenGL_DrawVertexArray(u32 vertexArray, u64 indicesOffset, u64 numIndices);
