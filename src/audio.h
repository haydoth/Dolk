#pragma once
#include "common.h"
#include "types.h"

void Audio_Init();

u32 Audio_CreateBufferWAV(void* data, u32 dataSize, u32 sampleRate, u16 wavFormat, u16 numChannels, u16 bitsPerSample);

u32 Audio_CreateSource(u32 buffer);
void Audio_PlaySource(u32 source);

void Audio_SourceSetBuffer(u32 source, u32 buffer);
void Audio_SourceSetGain(u32 source, f32 gain);
void Audio_SourceSetPitch(u32 source, f32 pitch);
void Audio_SourceSetLooping(u32 source, bool looping);
void Audio_SourceSetPositionV(u32 source, float* position);
void Audio_SourceSetVelocityV(u32 source, float* velocity);
