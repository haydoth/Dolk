#pragma once

#include "arena.h"
#include "common.h"

typedef struct wav_data {
  void* PCMData;
  u32 PCMDataSize;
  u32 SampleRate;
  u16 Format;
  u16 NumChannels;
  u16 BitsPerSample;
} wav_data;

/* Provided the buffer is a valid WAV file, this copies data from the "data" chunk of the buffer to the arena
   and returns a struct with a pointer to it. If we didn't copy it we would have to retain the entire original
   buffer in memory and store a pointer to the data chunk. */ 
wav_data ReadWAVBuffer(void* buffer, u32 bufferSize, arena* _arena); 

