#include "common.h"
#include "arena.h"

typedef struct {
  char* CString;
  u64 Length, Capacity;
} string_builder;

string_builder sb_create(u64 capacity, arena* _arena)
void sb_append (string_builder* sb, char* str);
void sb_appendf(string_builder* sb, char* str);
void sb_append_null(string_builder* sb);

