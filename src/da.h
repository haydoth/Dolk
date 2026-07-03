#pragma once


// Dolk
#include "common.h"
#include "arena.h"
#include "types.h"

// typedef struct {
//   void** Items;
//   u64 Count;
//   u64 Capacity;
// } DynamicArray;


typedef struct {
  char** Items;
  u64 Count;
  u64 Capacity;
} CStrings;

typedef struct {
  i32* Items;
  u64 Count;
  u64 Capacity;
} I32s;

typedef struct {
  u32* Items;
  u64 Count;
  u64 Capacity;
} U32s;

typedef struct {
  f32* Items;
  u64 Count;
  u64 Capacity;
} F32s;


#define DA_INIT_CAP 16

void da_prep(u64 item_size, arena* _arena, void* da);

#define da_append(da, _arena)\
  (da_prep(sizeof((da).Items[0]), (_arena), &(da)), (da).Items + (da).Count++)
