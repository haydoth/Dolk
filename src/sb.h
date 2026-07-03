#include "common.h"
#include "arena.h"
#include "sv.h"

typedef struct {
  char* CString;
  u64 Length, Capacity;
} string_builder;

const char* sb_view_to_cstr(arena* _arena, string_view view);

void sb_append(string_builder* sb, arena* _arena, char* str, u64 len);
void sb_append_null(string_builder* sb, arena* _arena);

