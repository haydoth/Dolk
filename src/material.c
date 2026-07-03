#include "common.h"

void BindMaterial(Material* mat) {
  if(!mat->shader) {
    Shader* shader = GetShader(render, mat->shaderName);
    if(!shader) {
      DOLK_ERROR("Trying to use shader \"%s\", which doesn't exist!", mat->shaderName);
      return;
    }
  }

  Shader_Use(mat->shader->handle);
  for(int i = 0; i < mat->propertyCount; ++i) {
    switch(mat->properties[i].kind) {
    case MAT_PROP_FLOAT:
      Shader_SetUniformFloat(mat->shader->handle, mat->properties[i].name. mat->properties[i].as.Float);
      break;
    case MAT_PROP_VEC3:
      Shader_SetUniformVec3(mat->shader->handle, mat->properties[i].name. mat->properties[i].as.Vector3);
      break;
    default: break;
    }
  }
}

Material ReadMaterialBuffer(void* buffer, unsigned long long bufferSize) {
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
    
    if(sv_cmp(token, sv("#material"))) {
      ret.name = (const char*)name.CString;
    }
    if(sv_cmp(token, sv("#shader"))) {
      ret.shaderName = (const char*)name.CString;
    }
    if(sv_cmp(token, sv("float"))) {
      string_view value = sv_split(&line, ' ', false);
      ret.properties[propertyCount++] = (MaterialProperty) {
	.name = (const char*)name.CString,
	.kind = MAT_PROP_FLOAT,
	.as.Float = (f32)atof((const char*)value.CString)
      };
    }
    if(sv_cmp(token, sv("vec3"))) {
      string_view x = sv_split(&line, ' ', false);
      string_view y = sv_split(&line, ' ', false);
      string_view z = sv_split(&line, ' ', false);
      ret.properties[propertyCount++] = (MaterialProperty) {
	.name = (const char*)name.CString,
	.kind = MAT_PROP_VEC3,
	.as.Vector3 = {
	  (f32)atof((const char*)x.CString),
	  (f32)atof((const char*)y.CString),
	  (f32)atof((const char*)z.CString)
	}
      };
    }
  }
  return ret;
}
