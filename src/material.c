#include "material.h"
#include "shader.h"
#include "sv.h"
#include "sb.h"

void BindMaterial(Material* mat) {
  Shader_Use(mat->shader->handle);
  for(int i = 0; i < mat->propertyCount; ++i) {
    switch(mat->properties[i].kind) {
    case MAT_PROP_FLOAT:
      Shader_SetUniformFloat(mat->shader->handle, mat->properties[i].name, mat->properties[i].as.Float);
      break;
    case MAT_PROP_VEC3:
      Shader_SetUniformVec3(mat->shader->handle, mat->properties[i].name, mat->properties[i].as.Vector3);
      break;
    default: break;
    }
  }
}

// A more sophisticated parser would at least implement a lexer so I didnt have to trim everything in this primitive fashion
Material
ReadMaterialBuffer(void* buffer, unsigned long long bufferSize, arena* persistent) {
  ASSERT(buffer && bufferSize);
  string_view str = {(char*)buffer, bufferSize};
  Material ret = {0};
  
  while(str.Length > 0) {
    string_view line = sv_split(&str, '\n', false);
    sv_trim(&line);
    
    string_view token = sv_split(&line, ' ', false);
    string_view name = sv_split(&line, ' ', false);
    sv_trim(&token);
    sv_trim(&name);

    if(sv_cmp(token, sv("float"))) {
      string_view value = sv_split(&line, ' ', false);
      ret.properties[ret.propertyCount++] = (MaterialProperty) {
	.name = sb_view_to_cstr(persistent, name),
	.kind = MAT_PROP_FLOAT,
	.as.Float = (f32)atof(sb_view_to_cstr(persistent, value))
      };
    }
    if(sv_cmp(token, sv("vec3"))) {
      string_view x = sv_split(&line, ' ', false);
      string_view y = sv_split(&line, ' ', false);
      string_view z = sv_split(&line, ' ', false);
      ret.properties[ret.propertyCount++] = (MaterialProperty) {
	.name = sb_view_to_cstr(persistent, name),
	.kind = MAT_PROP_VEC3,
	.as.Vector3 = {
	  (f32)atof(sb_view_to_cstr(persistent, x)),
	  (f32)atof(sb_view_to_cstr(persistent, y)),
	  (f32)atof(sb_view_to_cstr(persistent, z))
	}
      };
    }
  }
  return ret;
}
