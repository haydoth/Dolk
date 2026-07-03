#pragma once
#include "types.h"
#include "audio.h"
#include "wav.h"
#include "file_io.h"

// returns OpenAL buffer
inline u32 LoadWAVSound(char* path) {
  arena_temp scratch = GetScratch(0, 0);

  file_buffer buf = ReadEntireFile(path, scratch.Arena);
  wav_data data = ReadWAVBuffer(buf.Buffer, buf.Size, scratch.Arena);
  u32 audioBuffer = Audio_CreateBufferWAV(data.PCMData, data.PCMDataSize,
					  data.SampleRate, data.Format,
					  data.NumChannels, data.BitsPerSample);
  ReleaseScratch(scratch); 
  return audioBuffer;
}
