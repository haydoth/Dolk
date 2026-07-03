#include "obj.h"
#include "sv.h"
#include "ht.h"

// STD
#include <stdlib.h>
#include <math.h>

// CGLM
#include <cglm/struct.h>


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

#define szudzik(a, b) a >= b ? a * a + a + b : a + b * b
#define hash_u32_3(a, b, c) szudzik(szudzik(a, b), c)

typedef struct {
  ht_header Header;
  u32 v, vt, vn, index;
} obj_index;

typedef struct {
  obj_index* Items;
  u64 Count;
  u64 Capacity;
} OBJIndices;

internal u32
obj_index_lookup(u32 v, u32 vt, u32 vn, OBJIndices objIndices) {

 obj_index* ret = ht_find(&objIndices, (u64)hash_u32_3(v, vt, vn));
 if(ret) return ret->index; else return 0;
}

typedef struct {
  vec3s *Items;
  u64 Count;
  u64 Capacity;
} Vector3s;

typedef struct {
  vec2s *Items;
  u64 Count;
  u64 Capacity;
} Vector2s;

obj_data
ReadOBJBuffer(void* buffer, u64 bufferSize, arena* persistent) {

  ASSERT(buffer);
  string_view str = {(char*)buffer, bufferSize};

  obj_data data = {0};
  
  arena_temp scratch = GetScratch((arena*[]){persistent}, 1);
  
  Vector3s positions = {0};
  Vector3s normals = {0};
  Vector2s textureCoordinates = {0};
  
  U32s positionIndices = {0};
  U32s textureCoordinateIndices = {0};
  U32s normalIndices = {0};

  u64 lineIndex = 0;
  while(str.Length > 0) {
    //DOLK_LOG("Analyzing line %llu...\n", lineIndex);
    lineIndex++;
    string_view line = sv_split(&str, '\n', false);

    //DOLK_LOG(SV_Fmt"\n", SV_Arg(line));
    while(line.Length > 0) {
      //DOLK_LOG("Line length: %llu\n", line.Length);
      string_view word0 = sv_split(&line, ' ', false);
      string_view word1 = sv_split(&line, ' ', false);
      string_view word2 = sv_split(&line, ' ', false);
      string_view word3 = sv_split(&line, ' ', false);
      //DOLK_LOG("|"SV_Fmt"| |"SV_Fmt"| |"SV_Fmt"| |"SV_Fmt"|\n", SV_Arg(word0), SV_Arg(word1), SV_Arg(word2), SV_Arg(word3));
      sv_trim(&word0);
      sv_trim(&word1);
      sv_trim(&word2);
      sv_trim(&word3);

      if(sv_cmp(word0, sv("v"))) {
	vec3s pos = {sv_to_f32(word1), sv_to_f32(word2), sv_to_f32(word3)};
	*da_append(positions, scratch.Arena) = pos;
      }
      else if(sv_cmp(word0, sv("vt"))) {
	vec2s tc = {sv_to_f32(word1), sv_to_f32(word2)};
	*da_append(textureCoordinates, scratch.Arena) = tc;
      }
      else if(sv_cmp(word0, sv("vn"))) {
	vec3s nrm = {sv_to_f32(word1), sv_to_f32(word2), sv_to_f32(word3)};
	*da_append(normals, scratch.Arena) = nrm;
      }
      else if(sv_cmp(word0, sv("f"))) {
	string_view pi0 = sv_split(&word1, '/', false);
	string_view tci0 = sv_split(&word1, '/', false);
	string_view ni0 = sv_split(&word1, '/', false);

	string_view pi1 = sv_split(&word2, '/', false);
	string_view tci1 = sv_split(&word2, '/', false);
	string_view ni1 = sv_split(&word2, '/', false);

	string_view pi2 = sv_split(&word3, '/', false);
	string_view tci2 = sv_split(&word3, '/', false);
	string_view ni2 = sv_split(&word3, '/', false);

	sv_trim(&pi0); sv_trim(&tci0); sv_trim(&ni0);
	sv_trim(&pi1); sv_trim(&tci1); sv_trim(&ni1);
	sv_trim(&pi2); sv_trim(&tci2); sv_trim(&ni2);

	*da_append(positionIndices, scratch.Arena) = sv_to_u32(pi0) ;
	*da_append(positionIndices, scratch.Arena) = sv_to_u32(pi1) ;
	*da_append(positionIndices, scratch.Arena) = sv_to_u32(pi2) ;

	if(tci0.Length > 0 && tci1.Length > 0 && tci2.Length > 0) {
	  *da_append(textureCoordinateIndices, scratch.Arena) = sv_to_u32(tci0);
	  *da_append(textureCoordinateIndices, scratch.Arena) = sv_to_u32(tci1);
	  *da_append(textureCoordinateIndices, scratch.Arena) = sv_to_u32(tci2);
	}
	*da_append(normalIndices, scratch.Arena) = sv_to_u32(ni0);
	*da_append(normalIndices, scratch.Arena) = sv_to_u32(ni1);
	*da_append(normalIndices, scratch.Arena) = sv_to_u32(ni2);
      }
    }
  }
  
  OBJIndices lookupList = {0};
  
  bool hasUVs = textureCoordinateIndices.Count > 0;
  if(hasUVs) {
    for(u64 i = 0; i < positionIndices.Count; ++i) {
      u32 v = positionIndices.Items[i];
      u32 vt = textureCoordinateIndices.Items[i];
      u32 vn = normalIndices.Items[i];

      u32 ret = obj_index_lookup(v, vt, vn, lookupList);
      if(ret) {
	*da_append(data.Indices, persistent) = ret;
      }
      else {
	*da_append(data.Indices, persistent) = (u32)lookupList.Count;
	obj_index objIndex = {v, vt, vn, (u32)lookupList.Count};
	*da_append(lookupList, persistent) = objIndex;
	//ht_insert(&lookupList, persistent, (u64)hash_u32_3(v, vt, vn), &objIndex);

	*da_append(data.Vertices, persistent) = positions.Items[v - 1].x;
	*da_append(data.Vertices, persistent) = positions.Items[v - 1].y;
	*da_append(data.Vertices, persistent) = positions.Items[v - 1].z;
	*da_append(data.Vertices, persistent) = textureCoordinates.Items[vt - 1].x;
	*da_append(data.Vertices, persistent) = textureCoordinates.Items[vt - 1].y;
	*da_append(data.Vertices, persistent) = normals.Items[vn - 1].x;
	*da_append(data.Vertices, persistent) = normals.Items[vn - 1].y;
	*da_append(data.Vertices, persistent) = normals.Items[vn - 1].z;
      }
    }

    *da_append(data.Format, persistent) = 3;
    *da_append(data.Format, persistent) = 2;
    *da_append(data.Format, persistent) = 3;
  }
  else {    

    for(u64 i = 0; i < positionIndices.Count; ++i) {
      u32 v = positionIndices.Items[i];
      u32 vn = normalIndices.Items[i];

      u32 ret = obj_index_lookup(v, 0, vn, lookupList);
      if(ret) {
	*da_append(data.Indices, persistent) = ret;
      }
      else {
	*da_append(data.Indices, persistent) = (u32)lookupList.Count;
	obj_index objIndex = {v, 0, vn, (u32)lookupList.Count};
	*da_append(lookupList, persistent) = objIndex;
	  //ht_insert(&lookupList, persistent, (u64)hash_u32_3(v, 0, vn), &objIndex);

	*da_append(data.Vertices, persistent) = positions.Items[v - 1].x;
	*da_append(data.Vertices, persistent) = positions.Items[v - 1].y;
	*da_append(data.Vertices, persistent) = positions.Items[v - 1].z;
	*da_append(data.Vertices, persistent) = normals.Items[vn - 1].x;
	*da_append(data.Vertices, persistent) = normals.Items[vn - 1].y;
	*da_append(data.Vertices, persistent) = normals.Items[vn - 1].z;
      }
    }    
    *da_append(data.Format, persistent) = 3;
    *da_append(data.Format, persistent) = 3;
  }

  ReleaseScratch(scratch);
  return data;
}
