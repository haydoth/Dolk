#include "arena.h"

// STD
#include <stdio.h>

// Win32
#include <windows.h>

bool
is_power_of_two(u64 x) {
	return (x & (x-1)) == 0;
}

u64
align_forward(u64 ptr, u64 align) {
       u64 p, a, modulo;

	ASSERT(is_power_of_two(align));

	p = ptr;
	a = (u64)align;
	// Same as (p % a) but faster as 'a' is a power of two
	modulo = p & (a-1);

	if (modulo != 0) {
		// If 'p' address is not aligned, push the address to the
		// next value which is aligned
		p += a - modulo;
	}
	return p;
}

arena
arena_create() {

  u64 committed = Megabytes(64);
  u64 reserved = Gigabytes((u64)64);
  // absurdly large reservation of virtual address space guarantees memory contiguity
  // as arena expands

  void* buffer = VirtualAlloc(0, reserved, MEM_RESERVE, PAGE_NOACCESS);
  ASSERT(buffer);
  VirtualAlloc(buffer, committed, MEM_COMMIT, PAGE_READWRITE);
  ASSERT(buffer);
  memset(buffer, 0, committed);
  
  return (arena) {(u8*)buffer, 0, committed, reserved};
}


void*
arena_push_align(arena* a, u64 size, u64 alignment)
{
  u64 current = (u64)a->Buffer + a->Offset;
  u64 aligned = align_forward(current, alignment);
  u64 newOffset = (aligned - (u64)a->Buffer) + size;

  if(newOffset > a->Committed) {
    printf("Arena used too much memory");
    ASSERT(false); // Nothing should use this much memory, for now
  }

  a->Offset = newOffset;
  //  memset((void*)aligned, 0, size);
  return (void*)aligned;
}

void*
arena_write_align(arena* a, u64 size, u64 alignment, void* data) {
  void* ptr = arena_push_align(a, size, alignment);
  memcpy(ptr, data, size);
  return ptr;
}

void
arena_clear(arena* a) {
  a->Offset = 0;
  // TODO: if the arena had expanded, MEM_DECOMMIT back to the default arena size
}


void arena_free(arena* a) {
  VirtualFree(a->Buffer, 0, MEM_RELEASE);
}

arena_temp arena_temp_begin(arena* _arena) {
  return (arena_temp) {_arena, _arena->Offset};
}

void arena_temp_end(arena_temp temp) {
  temp.Arena->Offset = temp.Offset;
}

#define NUM_MAX_SCRATCH_ARENAS 16
global arena ScratchArenas[NUM_MAX_SCRATCH_ARENAS];
global u64 ScratchArena_Count;

// pass in persistent arenas that are used in the current codepath
arena_temp GetScratch(arena **conflicts, u64 conflictCount) {

  for(u64 i = 0; i < ScratchArena_Count; ++i) {
    bool isInUse = false;
    for(u64 j = 0; j < conflictCount; ++j) {
      if(conflicts[j] == ScratchArenas + i) isInUse = true;
    }
    if(!isInUse) return arena_temp_begin(ScratchArenas + i);
  }
  if(ScratchArena_Count >= NUM_MAX_SCRATCH_ARENAS) { ASSERT(false); }
  arena newScratch = arena_create();
  ScratchArenas[ScratchArena_Count++] = newScratch;
  return arena_temp_begin(&ScratchArenas[ScratchArena_Count - 1]);
}
