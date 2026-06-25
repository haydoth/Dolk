#include "sb.h"

#define SB_INIT_CAPACITY 256

void
sb_append(string_builder* sb, arena* _arena, char* str) {
  u64 len = strlen(str);
  if(sb->Length + len > sb->Capacity) {
    sb->Capacity *= 2;
    if(sb->Capacity == 0) {
      // init
      sb->Capacity = SB_INIT_CAPACITY;
      sb->CString = arena_push(_arena, sb->Capacity);
    }
    else {
      // move
      char* temp = sb->CString;
      sb->CString = arena_push(_arena, sb->Capacity);
      memcpy(sb->CString, temp, sb->Length);
    }
  }
  memcpy(sb->CString + sb->Length, str, len);
}

void
sb_appendf(string_builder* sb, arena* _arena, char* str) {}

void
sb_append_null(string_builder* sb, arena* _arena) {
  sb_append(sb. '/0');
}
