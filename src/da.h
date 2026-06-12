#pragma once

// STD
#include <stdlib.h>

// Dolk
#include "common.h"
#include "arena.h"

#define DA_MAX_SIZE Megabytes(8)

#define da(T) T*

#define list(T) T*

typedef struct da_header {u64 Count; arena Arena;} da_header;
typedef struct list_header {u64 Count; u64 Capacity;} list_header;

// NOTE: I've been thinking about how to avoid using malloc and free here. The problem being that arenas don't play well with the
// append/remove logic of dynamic arrays.
// Maybe it is better to just enforce the rule that all dynamic arrays are to be small in size and set a hard cap?

// NOTE: Upon further reflection I'm considering renaming dynamic arrays to lists since they have to be initialized with a capacity

#define __list_header(list) ((list_header*)(list) - 1)

#define list_init(list, capacity, arena)\
do {\
  if(!list)\
  {\
    u64 totalSize = sizeof(list_header) + (sizeof(*list) * capacity);\
    list_header* header = arena_push((arena), totalSize);\
    header->Capacity = capacity;\
    /*Count is already 0*/	\
    list = (void*)(header + 1);\
  }\
} while(0)

#define list_append(list, item)\
do {\
  ASSERT(list);\
  list_header* header = __list_header(list);\
  if(header->Count == header->Capacity) {\
    DOLK_ERROR("List overflow!");			\
    header->Count = 0;\
  }							\
  (list)[__list_header(list)->Count++] = (item);	\
} while(0)

#define list_len(list) __list_header(list)->Count
  

#define da_append(da, item)						\
  do {									\
    if(!da) {								\
      arena _arena = arena_create();					\
      da_header* header = (da_header*)_arena.Buffer;			\
      header->Count = 0;						\
      header->Arena = _arena;						\
      (da) = (void*)(header + 1);					\
    }									\
    da_header* header = (da_header*)(da) - 1;				\
    arena_push(&(header->Arena), sizeof(item));				\
    (da)[header->Count++] = (item);					\
  } while(0)


#define da_len(da) ((da) ? ((da_header*)(da) - 1)->Count : 0)

// NOTE: This will invalidate all handles to elements after the index
#define da_remove(da, index)				\
  do {							\
  if(da) {						\
    if(index < da_len(da)) {				\
      memmove(da + index, da + index + 1, sizeof(*da) * (da_len(da) - (index + 1))); \
    }							\
  }							\
} while(0)

#define da_erase(da, index)				\
  do {							\
    if(da) { if(index < da_len(da)) { memset(da + index, 0, sizeof(*da)); }} \
} while(0)

// NOTE: Could be made more efficient by memcopying the source DA to the end of the  destination
#define da_append_da(dest, src)\
  do {									\
      if(src) { for(u64 i = 0; i < da_len(src); i++) da_append((dest), (src)[i]); } \
} while(0)

#define da_clear(da) do { arena_clear(&(((da_header*)(da) - 1)->Arena)); ((da_header*)(da) - 1)->Count = 0;} while(0)
#define da_free(da) do { arena_free(&(((da_header*)(da) - 1)->Arena)); ((da_header*)(da) - 1)->Count = 0;} while(0)
