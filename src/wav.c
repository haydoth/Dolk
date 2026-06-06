#include "wav.h"

#include <string.h>

internal wav_data nil_wav_data = {0};

wav_data
ReadWAVBuffer(void* buffer, u32 bufferSize, arena* _arena)
{
  ASSERT(buffer && bufferSize);
  
  if(strncmp((const char*)buffer, "RIFF", 4)     != 0) { DOLK_ERROR("Failed to read WAV buffer!"); return nil_wav_data; }
  if(strncmp((const char*)buffer + 8, "WAVE", 4) != 0) { DOLK_ERROR("Failed to read WAV buffer!"); return nil_wav_data; }
  
  u32 index = 12;
  bool found_format = 0, found_data = 0;
  wav_data wav = {0};
  while(index + 4 < bufferSize && (!found_format || !found_data))
  {
    u8* chunk = (u8*)buffer + index; // first 4 bytes denote the ID of the chunk
    u32 dataSize = *(u32*)(chunk + 4);
    u32 chunkSize = dataSize + 8;
    if(chunkSize & 1) chunkSize += 1;
    u8* data = chunk + 8;
    
    if(strncmp((const char*)chunk, "fmt ", 4) == 0)
    {
      wav.Format        = *(u16*)(data + 0);
      wav.NumChannels   = *(u16*)(data + 2);
      wav.SampleRate    = *(u32*)(data + 4);
      wav.BitsPerSample = *(u16*)(data + 14);
      
      found_format = true;
    }
    if(strncmp((const char*)chunk, "data", 4) == 0)
    {
      wav.PCMDataSize = dataSize;
      wav.PCMData = arena_push(_arena, dataSize);
      memcpy(wav.PCMData, data, dataSize);
      
      found_data = true;
    }
    index += chunkSize;
  }

  return wav;
}


