#include "sv.h"

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


sv_split_result
sv_split(string_view sv, u64 index) 
{
  string_view left = {sv.CString, index};
  string_view right = {sv.CString + index, sv.Length - index};

  return (sv_split_result) {left, right};
}

bool
sv_cmp(string_view a, string_view b)
{
  if(a.Length != b.Length) return false;
  
  return (strncmp(a.CString, b.CString, a.Length) == 0);
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
