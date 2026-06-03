#pragma once

#include "arena.h"
#include "common.h"

// NOTE: only supports files of less than 4GB size as of now

// NOTE: interface is coupled with arena. This should potentially be reconsidered in the future

typedef struct file_buffer { void* Buffer; u32 Size; } file_buffer;

bool         WriteEntireFile(char* path, void* buffer, u32 bufferSize);
file_buffer  ReadEntireFile (char* path, arena* a);
