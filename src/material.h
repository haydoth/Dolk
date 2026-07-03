#pragma once

#include "types.h"

void BindMaterial(Material* mat);
Material ReadMaterialBuffer(void* buffer, u64 bufferSize);
