#pragma once

#include "common.h"

#define ARENA_DEFAULT_ALIGNMENT (sizeof(void*)*2)

typedef struct arena {

  u8* Buffer;
  u64 Size;
  u64 Committed;
  u64 Reserved;

} arena;

arena arena_create();
void* arena_push_align(arena* a, u64 size, u64 alignment);
void arena_clear(arena* a);
void arena_free(arena* a);

#define arena_push(arena, size)\
  arena_push_align((arena), (size), ARENA_DEFAULT_ALIGNMENT)
#define arena_push_many(arena, type, count)\
  arena_push_align((arena), sizeof(type)*(count), ARENA_DEFAULT_ALIGNMENT);


