#pragma once

#include "common.h"
#include "da.h"
#include "arena.h"

typedef struct {
  da(f32) Vertices;
  da(u32) Indices;
  da(u32) Format;
} obj_data;

obj_data ReadOBJBuffer(void* buffer, u64 bufferSize);
