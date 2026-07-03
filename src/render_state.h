#pragma once
#include "types.h"

Shader* GetShader(RenderState* render, const char* name);
Material* GetMaterial(RenderState* render, const char* name);

#include "common.h"
#include "shader.h"
#include "obj.h"
#include "file_io.h"
#include "arena.h"
#include "vertex_array.h"
#include "buffer.h"
#include "renderable.h"
#include "material.h"

Shader* GetShader(RenderState* render, const char* name);
Material* GetMaterial(RenderState* render, const char* name);

Mesh LoadMesh(RenderState* render, char* path);
Shader LoadShader(RenderState* render, char* path);
Material LoadMaterial(RenderState* render, char* path);
