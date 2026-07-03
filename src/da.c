#include "da.h"

#include <string.h>
#include <stdlib.h>

// Dynamic array memory layout
// [8][8][8]
// ptr, count, capacity

void da_prep(u64 item_size, arena* _arena, void* da) {
  u64* count = ((u64*)da) + 1;
  u64* capacity = ((u64*)da) + 2;
  if(*count < *capacity) return;

  if(*capacity == 0) {
    *(void**)da = arena_push(_arena, item_size * DA_INIT_CAP);
    *capacity = DA_INIT_CAP;
  }
  else {
    void *temp = arena_push(_arena, item_size * (*capacity) * 2);
    memcpy(temp, *(void**)da, (*count) * item_size);
    *(void**)da = temp;
    
    *capacity *= 2;
  }
}
