#include "dolk.h"

#include "common.h"
#include "audio.h"
#include "arena.h"
#include "file_io.h"
#include "wav.h"

u32 LoadWAV(char* path, arena* _arena) {
  arena_temp temp = arena_temp_begin(_arena);

  file_buffer buf = ReadEntireFile(path, _arena);
  wav_data data = ReadWAVBuffer(buf.Buffer, buf.Size, _arena);
  u32 audioBuffer = Audio_CreateBufferWAV(data.PCMData, data.PCMDataSize,
					  data.SampleRate, data.Format,
					  data.NumChannels, data.BitsPerSample);

  arena_temp_end(temp);
  return audioBuffer;
}

void Init() {
  Audio_Init();

  arena myArena = arena_create();
  u32 audioBuffer = LoadWAV("../music.wav", &myArena);
  u32 audioSource = Audio_CreateSource(audioBuffer);
  
  Audio_PlaySource(audioSource);
}

void Update(f64 delta)
{
  UNUSED(delta);
}
