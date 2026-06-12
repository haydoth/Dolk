#include <cglm/cglm.h> 

#include "common.h"

void Audio_Init();

u32 Audio_CreateBufferWAV(void* data, u32 dataSize, u32 sampleRate, u16 wavFormat, u16 numChannels, u16 bitsPerSample);

u32 Audio_CreateSource(u32 buffer);
void Audio_PlaySource(u32 source);

void Audio_SourceSetBuffer(u32 source, u32 buffer);
void Audio_SourceSetGain(u32 source, f32 gain);
void Audio_SourceSetPitch(u32 source, f32 pitch);
void Audio_SourceSetLooping(u32 source, bool looping);
void Audio_SourceSetPosition(u32 source, vec3 position);
void Audio_SourceSetVelocity(u32 source, vec3 velocity);
