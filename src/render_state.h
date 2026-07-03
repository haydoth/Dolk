#pragma once
#include "types.h"
#include "arena.h"

RenderableObject* GetRenderable(RenderState* render, const char* name);

Mesh* GetMesh(RenderState* render, const char* name);
Shader* GetShader(RenderState* render, const char* name);
Material* GetMaterial(RenderState* render, const char* name);

Mesh* LoadMesh(RenderState* render, char* path, const char* name);
Shader* LoadShader(RenderState* render, char* path, const char* name);
Material* LoadMaterial(RenderState* render, arena* persistent, char* path, const char* name, Shader* shader);
