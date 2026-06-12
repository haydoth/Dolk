#pragma once

#include "common.h"

#define ARENA_DEFAULT_ALIGNMENT (sizeof(void*)*2)

typedef struct arena {

  u8* Buffer;
  u64 Offset;
  u64 Committed;
  u64 Reserved;

} arena;

arena arena_create();
void* arena_push_align(arena* a, u64 size, u64 alignment);
void* arena_write_align(arena* a, u64 size, u64 alignment, void* data);
void  arena_clear(arena* a);
void  arena_free (arena* a);

#define arena_push(arena, size)\
  arena_push_align((arena), (size), ARENA_DEFAULT_ALIGNMENT)
#define arena_push_many(arena, type, count)\
  arena_push_align((arena), sizeof(type)*(count), ARENA_DEFAULT_ALIGNMENT)

#define arena_write(arena, size, data)				\
  arena_write_align((arena), (size), ARENA_DEFAULT_ALIGNMENT, (data))

typedef struct arena_temp { arena* Arena; u64 Offset; } arena_temp;

arena_temp arena_temp_begin(arena* _arena);
void arena_temp_end(arena_temp temp);

#define get_scratch() arena_create()
#define release_scratch(arena) arena_free((arena))
