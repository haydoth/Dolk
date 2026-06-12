#include "obj.h"

#include <stdlib.h>
#include <math.h>
#include <cglm/struct.h>

#include "sv.h"

u32
sv_to_u32(string_view sv) {

  sv_trim(&sv);
  u32 ret = 0;
  for(u64 i = 0; i < sv.Length; ++i) {
    char c = sv.CString[i];
    if(!isdigit(c)) return 0;
    ret = ret * 10 + (c - '0');
  }
  return ret;
}

f32
sv_to_f32(string_view sv) {

  sv_trim(&sv);
  f32 ret = 0;
  f32 fractional = 0;
  bool passedDot = false;
  bool negative = (sv.CString[0] == '-');
  u64 fractionalIndex = 0;
  
  for(u64 i = 0; i < sv.Length; ++i) {
    char c = sv.CString[i];
    if(!isdigit(c)) {
      if(c == '.') {
	passedDot = true;
	fractionalIndex = i + 1;
	continue;
      }
      if(negative && i == 0) continue;
      return 0;
    }
    
    if(passedDot) {
      u64 j = i - fractionalIndex;
      fractional = fractional + ((f32)(c - '0') * (f32)pow(0.1, (f32)j + 1));
    } else {
      ret = ret * 10 + (f32)(c - '0');
    }
  }
  f32 multiplier = negative ? -1.0f : 1.0f;
  return (ret + fractional) * multiplier;
}

typedef struct {u32 v, vt, vn, index;} obj_index;

internal u32
obj_index_lookup(u32 v, u32 vt, u32 vn, da(obj_index) objIndices) {
  for(u64 i = 0; i < da_len(objIndices); ++i) {
    obj_index index = objIndices[i];
    if(index.v == v && index.vt == vt && index.vn == vn) return index.index; 
  }
  return 0;
}

obj_data
ReadOBJBuffer(void* buffer, u64 bufferSize) {
    
  string_view str = {(char*)buffer, bufferSize};
  da(string_view) lines = sv_split_by_delim(str, '\n', false);

  obj_data data = {0};

  arena scratch = get_scratch();
  
  vec3s *positions = 0, *normals = 0;
  vec2s *textureCoordinates = 0;

  list_init(positions, 100000, &scratch);
  list_init(textureCoordinates, 100000, &scratch);
  list_init(normals, 100000, &scratch);
  
  u32 *positionIndices = 0, *textureCoordinateIndices = 0, *normalIndices = 0;

  list_init(positionIndices, 500000, &scratch);
  list_init(textureCoordinateIndices, 500000, &scratch);
  list_init(normalIndices, 500000, &scratch);
  
  for(u64 i = 0; i < da_len(lines); ++i) {
    
    string_view line = lines[i];
    sv_trim(&line);

    da(string_view) words = sv_split_by_delim(line, ' ', false);
    if(sv_cmp(words[0], sv("v"))) {
      //DOLK_LOG("%f, %f, %f\n", sv_to_f32(words[1]), sv_to_f32(words[2]), sv_to_f32(words[3]));
      vec3s pos = {sv_to_f32(words[1]), sv_to_f32(words[2]), sv_to_f32(words[3])};
      da_append(positions, pos);
    }
    else if(sv_cmp(words[0], sv("vn"))) {

      vec3s normal  = {sv_to_f32(words[1]), sv_to_f32(words[2]), sv_to_f32(words[3])};      
      da_append(normals, normal);
    }
    else if(sv_cmp(words[0], sv("vt"))) {
      
      vec2s texCoord = {sv_to_f32(words[1]), sv_to_f32(words[2])};
      da_append(textureCoordinates, texCoord);
    }
    else if(sv_cmp(words[0], sv("f"))) {
      
      for(u64 j = 1; j < 4; ++j) {
	da(string_view) vertex = sv_split_by_delim(words[j], '/', false);
	string_view posIndex = vertex[0];
	string_view tcIndex = vertex[1];
	string_view normIndex = vertex[2];
	// DOLK_LOG(SV_Fmt", "SV_Fmt"\n", SV_Arg(words[j]), SV_Arg(posIndex));
	da_append(positionIndices, sv_to_u32(posIndex));
	if(tcIndex.Length > 0) da_append(textureCoordinateIndices, sv_to_u32(tcIndex));
	da_append(normalIndices, sv_to_u32(normIndex));
      }
    }
    else {
      DOLK_INFO("OBJ parser ignored line: "SV_Fmt"\n", SV_Arg(line));
    }
    da_free(words);
  }

  da(obj_index) lookup_list = 0;
  
  bool hasUVs = da_len(textureCoordinateIndices) > 0;
  if(hasUVs) {
    for(u64 i = 0; i < da_len(positionIndices); ++i) {
      u32 v = positionIndices[i];
      u32 vt = textureCoordinateIndices[i];
      u32 vn = normalIndices[i];

      u32 ret = obj_index_lookup(v, vt, vn, lookup_list);
      if(ret) {
	da_append(data.Indices, ret);
      }
      else {
	da_append(data.Indices, (u32)da_len(lookup_list));
	obj_index objIndex = {v, vt, vn, (u32)da_len(lookup_list)};
	da_append(lookup_list, objIndex);
	da_append(data.Vertices, positions[v - 1].x);
	da_append(data.Vertices, positions[v - 1].y);
	da_append(data.Vertices, positions[v - 1].z);
      
	da_append(data.Vertices, textureCoordinates[vt - 1].x);
	da_append(data.Vertices, textureCoordinates[vt - 1].y);

	da_append(data.Vertices, normals[vn - 1].x);
	da_append(data.Vertices, normals[vn - 1].y);
	da_append(data.Vertices, normals[vn - 1].z);
      }
    }

    da_append(data.Format, 3);
    da_append(data.Format, 2);
    da_append(data.Format, 3);
  }
  else {    

    DOLK_INFO("Kill yourself");
    ASSERT(false);

    da_append(data.Format, 3);
    da_append(data.Format, 3);
  }

  da_free(lookup_list);
  
  return data;
}
