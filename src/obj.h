#pragma once
#include "common.h"
#include "da.h"
#include "arena.h"
#include "sv.h"

typedef struct {
  F32s Vertices;
  U32s Indices;
  U32s Format;
} obj_data;

obj_data ReadOBJBuffer(void* buffer, u64 bufferSize, arena* persistent);
