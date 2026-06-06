#include "audio.h"

#include <AL/al.h>
#include <AL/alc.h>

internal ALCdevice* Device;
internal ALCcontext* Context;

void
Audio_Init() {

  Device = alcOpenDevice(0);
  ASSERT(Device);

  Context = alcCreateContext(Device, 0);
  ASSERT(Context);
  alcMakeContextCurrent(Context);

}

u32
Audio_CreateBufferWAV(void* data, u32 dataSize, u32 sampleRate, u16 wavFormat, u16 numChannels, u16 bitsPerSample) {

  if(wavFormat != 1) {DOLK_ERROR("Error loading WAV data: Only PCM format is supported!"); return 0;} 

  ALenum format = 0;
  if(numChannels == 1) {
    if(bitsPerSample == 8) {
      format = AL_FORMAT_MONO8;
    }
    if(bitsPerSample == 16) {
      format = AL_FORMAT_MONO16;
    }
  }
  else if(numChannels == 2) {
    if(bitsPerSample == 8) {
      format = AL_FORMAT_STEREO8;
    }
    if(bitsPerSample == 16) {
      format = AL_FORMAT_STEREO16;
    }
  }
  ASSERT(format);

  u32 buffer = 0;
  alGenBuffers(1, &buffer);

  alBufferData(buffer, format, data, dataSize, sampleRate);
  return buffer;
}

u32 Audio_CreateSource(u32 buffer) {

  u32 source = 0;
  alGenSources(1, &source);
  alSourcef(source, AL_GAIN, 0.1f);
  alSourcei(source, AL_BUFFER, buffer);
  return source;
}

void
Audio_PlaySource(u32 source) {

  alSourcePlay(source);
  
}
