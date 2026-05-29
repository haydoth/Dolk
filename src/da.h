#pragma once

// STD
#include <stdlib.h>

// Chisel
#include "common.h"

#define DA_INIT_CAPACITY 256

#define da(T) T*

typedef struct da_header {u64 ItemSize, Count, Capacity;} da_header;

#define da_append(da, item)\
  do {									\
    if(!da) {								\
      da_header* header = malloc(sizeof(*da) * DA_INIT_CAPACITY + sizeof(da_header)); \
      header->Count = 0;						\
      header->Capacity = DA_INIT_CAPACITY;				\
      da = (void*)(header + 1);						\
    }									\
    da_header* header = (da_header*)(da) - 1;				\
    if(header->Count >= header->Capacity) {				\
      header->Capacity *= 2;						\
      realloc(header, sizeof(*da)*header->Capacity + sizeof(da_header));\
      da = (void*)(header + 1);						\
    }									\
    (da)[header->Count++] = (item);					\
  } while(0)

#define da_len(da) ((da_header*)(da) - 1)->Count

// NOTE: Could be made more efficient by memcopying the source DA to the end of the  destination
#define da_append_da(dest, src)\
  do {									\
      if(src) { for(u64 i = 0; i < da_len(src); i++) da_append((dest), (src)[i]); } \
} while(0)

#define da_free(da) free((da_header*)(da) - 1);
