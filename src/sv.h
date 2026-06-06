#pragma once

// STD
#include <ctype.h>
#include <string.h>

// Chisel
#include "common.h"
#include "da.h"

typedef struct string_view {

  char* CString;
  u64 Length;

} string_view;

typedef struct sv_split_result { string_view Left, Right; } sv_split_result;

string_view sv(char* cStr);

void sv_chop_left(string_view* sv, u64 n);
void sv_chop_right(string_view* sv, u64 n);
void sv_trim_left(string_view* sv);
void sv_trim_right(string_view* sv);
void sv_trim(string_view* sv);

sv_split_result sv_split(string_view sv, u64 index);
bool sv_cmp(string_view a, string_view b);

da(string_view) sv_split_by_delim(string_view sv, char delim, bool includeDelim);

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)sv.Length, sv.CString

