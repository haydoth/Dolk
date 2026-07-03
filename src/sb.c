#include "sb.h"

#define SB_INIT_CAPACITY 16

const char* sb_view_to_cstr(arena* _arena, string_view view) {
  string_builder sb = {0};
  sb_append(&sb, _arena, view.CString, view.Length);
  sb_append_null(&sb, _arena);
  return (const char*)sb.CString;
}

void
sb_append(string_builder* sb, arena* _arena, char* str, u64 len) {
  while(sb->Length + len > sb->Capacity) {
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
sb_append_null(string_builder* sb, arena* _arena) {
  char* str = "\0";
  sb_append(sb, _arena, str, strlen(str));
}
