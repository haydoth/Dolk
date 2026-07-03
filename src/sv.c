#include "sv.h"

string_view
sv(char* cStr)
{
  return (string_view) {cStr, strlen(cStr)};
}

void
sv_chop_left(string_view* sv, unsigned long long  n)
{
  if(n > sv->Length) n = sv->Length;
  sv->Length  -= n;
  sv->CString += n;
}

void
sv_chop_right(string_view* sv, unsigned long long n)
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


unsigned long long
sv_word_count(string_view sv) {
  sv_trim(&sv);
  unsigned long long spaceCount = 0;
  for(unsigned long long i = 0; i < sv.Length; ++i) {
    char c = sv.CString[i];
    if(c == ' ') spaceCount += 1;
  }
  if(spaceCount == sv.Length) return 0;
  return spaceCount + 1;
}

unsigned long long
sv_line_count(string_view sv) {
  unsigned long long newLineCount = 0;
  for(unsigned long long i = 0; i < sv.Length; ++i) {
    char c = sv.CString[i];
    if(c == '\n') newLineCount += 1;
  }
  return newLineCount;
}

int
sv_cmp(string_view a, string_view b)
{
  if(a.Length != b.Length) return 0;
  
  return (strncmp(a.CString, b.CString, a.Length) == 0);
}

string_view
sv_split(string_view* sv, char delim, int includeDelim)
{
  unsigned long long index = 0;
  while(index < sv->Length) {
    if(sv->CString[index] == delim) {
      string_view result = {sv->CString, index + includeDelim};
      sv_chop_left(sv, index + 1);
      return result;
    }
    ++index; 
  }
  string_view result = *sv;
  sv_chop_left(sv, index);
  return result;
}
