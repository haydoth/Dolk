#pragma once

#include <ctype.h>
#include <string.h>

typedef struct string_view {

  char* CString;
  unsigned long long Length;

} string_view;

string_view sv(char* cStr);

void sv_chop_left(string_view* sv, unsigned long long n);
void sv_chop_right(string_view* sv, unsigned long long n);
void sv_trim_left(string_view* sv);
void sv_trim_right(string_view* sv);
void sv_trim(string_view* sv);

unsigned long long sv_word_count(string_view sv);
unsigned long long sv_line_count(string_view sv);
  
bool sv_cmp(string_view a, string_view b);

string_view sv_split(string_view* sv, char delim, bool includeDelim);



#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)sv.Length, sv.CString

