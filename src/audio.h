#include "common.h"

void Audio_Init();
u32 Audio_CreateBufferWAV(void* data, u32 dataSize, u32 sampleRate, u16 wavFormat, u16 numChannels, u16 bitsPerSample);
u32 Audio_CreateSource(u32 buffer);
void Audio_PlaySource(u32 source);


