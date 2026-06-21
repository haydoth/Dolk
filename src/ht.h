#pragma once

#include "common.h"
#include "arena.h"
#include "string.h"

#define HT_INIT_CAP 16
#define HT_MAX_LOAD_FACTOR 0.75
#define HT_MAX_PROBE_DEPTH 1024

// TODO: implement tombstones and element deletion

// NOTE: insert creates a copy, find gets a pointer

/*

  Usage:
  typedef struct { ht_header Header; [stuff you want to store] } entry_t;
  typedef struct { entry_t* Items; u64 Count; u64 Capacity; } hash_table_t;
  
 */

typedef struct {
  u64 Key;
  bool Occupied;
} ht_header;

inline uint64_t splitmix64(uint64_t key) {
  key ^= key >> 30;
  key *= 0xbf58476d1ce4e5b9ULL;
  key ^= key >> 27;
  key *= 0x94d049bb133111ebULL;
  key ^= key >> 31;
  return key;
}

#define hash_u64(key) splitmix64(key)

inline void* _ht_find(void* base, u64 k, size_t item_size, u64 cap) {

  if(cap == 0) return 0;
  ASSERT(IS_POWER_OF_TWO(cap));
  u64 index = hash_u64(k) & (cap - 1);

  ht_header* header = (ht_header*)((char*)base + (index * item_size));

  u64 counter = 0;
  while(header->Occupied) {
    if(header->Key == k) return header;
    if(counter >= HT_MAX_PROBE_DEPTH) return 0;
    ++counter;
    ++index;
    if(index >= cap) index = 0;
    header = (ht_header*)((char*)base + (index * item_size));
  }
  return 0;
}

#define ht_find(ht, k)\
  _ht_find((ht)->Items, (k), sizeof((ht)->Items[0]), (ht)->Capacity)

#define ht_init(ht, a)							\
  do {									\
    (ht)->Items = arena_push((a), sizeof((ht)->Items[0]) * HT_INIT_CAP); \
    (ht)->Count = 0;							\
    (ht)->Capacity = HT_INIT_CAP;					\
  } while(0)

inline void _ht_expand(void** items, u64* cap, u64* count, size_t item_size, arena* a) {
    u64 old_cap = *cap;
    void* old_items = *items;
    u64 new_cap = old_cap * 2;

    void* new_items = arena_push(a, item_size * new_cap);

    for (u64 i = 0; i < old_cap; i++) {
        ht_header* src = (ht_header*)((char*)old_items + i * item_size);
        if (!src->Occupied) continue;

        u64 index = hash_u64(src->Key) & (new_cap - 1);
        ht_header* dst = (ht_header*)((char*)new_items + index * item_size);

        u64 counter = 0;
        while (dst->Occupied) {
	    ++counter;
            ASSERT(counter < HT_MAX_PROBE_DEPTH);
            ++index;
            if (index >= new_cap) index = 0;
            dst = (ht_header*)((char*)new_items + index * item_size);
        }
        memcpy(dst, src, item_size);
    }
    *items  = new_items;
    *cap    = new_cap;
    *count  = *count; // unchanged, all occupied entries were copied
}

#define ht_expand(ht, a) \
  _ht_expand((void**)&(ht)->Items, &(ht)->Capacity, &(ht)->Count, sizeof((ht)->Items[0]), (a))

inline void _ht_insert(void* items, u64 capacity, u64* count, 
                        u64 k, void* v, size_t item_size) {
    u64 index = hash_u64(k) & (capacity - 1);
    ht_header* header = (ht_header*)((char*)items + index * item_size);
    u64 counter = 0;
    bool inserted = false;
    while (header->Occupied) {
        if (header->Key == k) {
            memcpy(header, v, item_size);
            header->Key = k;
            header->Occupied = true;
            inserted = true;
            break;
        }
        if (counter >= HT_MAX_PROBE_DEPTH) break;
        ++counter;
        ++index;
        if (index >= capacity) index = 0;
        header = (ht_header*)((char*)items + index * item_size);
    }
    if (!inserted) {
        memcpy(header, v, item_size);
        header->Key = k;
        header->Occupied = true;
        *count += 1;
    }
}

#define ht_insert(ht, a, k, v)                                              \
  do {                                                                      \
    if ((ht)->Capacity == 0) ht_init((ht), (a));                           \
    if ((ht)->Count >= (ht)->Capacity * HT_MAX_LOAD_FACTOR) ht_expand((ht), (a)); \
    _ht_insert((ht)->Items, (ht)->Capacity, &(ht)->Count,                  \
               (k), (v), sizeof((ht)->Items[0]));                     \
  } while(0)
