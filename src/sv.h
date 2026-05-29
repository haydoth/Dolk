#pragma once

#include "common.h"
#include "da.h"

typedef struct string_view {

  char* CString;
  u64 Length;

} string_view;

string_view
sv(char* cStr)
{
  return (string_view) {cStr, strlen(cStr)};
}

void sv_chop_left(string_view* sv, u64 n) {
  if(n > sv->Length) n = sv->Length;
  sv->Length  -= n;
  sv->CString += n;
}

void sv_chop_right(string_view* sv, u64 n) {
  if(n > sv->Length) n = sv->Length;
  sv->Length -= n;
}

da(string_view)
split_by_delim(string_view sv, char delim) {

  da(string_view) result = 0;
  u64 index = 0;
  while(index < sv.Length && sv.CString[index] != delim) {
    ++index; 
  }

  if(index < sv.Length) {
    string_view right = sv;
    sv_chop_left(&right, index + 1);

    string_view left = { sv.CString, index };

    da_append(result, left);
    da_append_da(result, split_by_delim(right, delim));
  }
  else da_append(result, sv);
  return result;
}

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)sv.Length, sv.CString

