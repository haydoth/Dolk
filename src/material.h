#pragma once
#include "types.h"
#include "arena.h"

void BindMaterial(Material* mat);
Material ReadMaterialBuffer(void* buffer, u64 bufferSize, arena* persistent);
