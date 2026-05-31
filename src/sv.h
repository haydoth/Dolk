#pragma once

// STD
#include <ctype.h>

// Chisel
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

void
sv_chop_left(string_view* sv, u64 n)
{
  if(n > sv->Length) n = sv->Length;
  sv->Length  -= n;
  sv->CString += n;
}

void
sv_chop_right(string_view* sv, u64 n)
{
  if(n > sv->Length) n = sv->Length;
  sv->Length -= n;
}

void
sv_trim_left(string_view* sv) {

  while(sv->Length > 0 && isspace(sv->CString[0])) {
    sv_chop_left(sv, 1);
  }
}

void
sv_trim_right(string_view* sv)
{
  while(sv->Length > 0 && isspace(sv->CString[sv->Length-1])) {
    sv_chop_right(sv, 1);
  }
}

void
sv_trim(string_view* sv)
{
  sv_trim_left(sv);
  sv_trim_right(sv);
}

typedef struct sv_split_result { string_view Left, Right; } sv_split_result;

sv_split_result
sv_split(string_view sv, u64 index) 
{
  string_view left = {left.CString, index};
  string_view right = {left.CString + index, sv.Length - index};

  return (sv_split_result) {left, right};
}

bool
sv_equal(string_view a, string_view b)
{
  if(a.Length != b.Length) return false;
  
  u64 i = 0;
  while(i < a.Length) {
    if(a.CString[i] != b.CString[i]) return false;
    ++i;
  }
  return true;
}

da(string_view)
sv_split_by_delim(string_view sv, char delim, bool includeDelim)
{
  da(string_view) result = 0;
  u64 index = 0;
  u64 last = 0;
  
  while(index < sv.Length) {

    if(sv.CString[index] == delim) {
      if(last > 0 && !includeDelim) ++last;
      string_view left = {sv.CString + last, index - last};
      da_append(result, left);
      last = index;
    }

    ++index; 
  }
  if(last > 0 && !includeDelim) ++last;
  string_view right = {sv.CString + last, index};
  da_append(result, right);
  
  return result;
}

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)sv.Length, sv.CString

