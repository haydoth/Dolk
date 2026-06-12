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
  alSourcei(source, AL_BUFFER, buffer);
  return source;
}

void
Audio_PlaySource(u32 source) {

  alSourcePlay(source);
}


void Audio_SourceSetBuffer(u32 source, u32 buffer) {
  alSourcei(source, AL_BUFFER, buffer);
}
void Audio_SourceSetGain(u32 source, f32 gain) {
  alSourcef(source, AL_GAIN, gain);
}
void Audio_SourceSetPitch(u32 source, f32 pitch) {
  alSourcef(source, AL_PITCH, pitch);
}
void Audio_SourceSetLooping(u32 source, bool looping) {
  alSourcei(source, AL_LOOPING, looping);
}
void Audio_SourceSetPosition(u32 source, vec3 position) {
  alSource3f(source, AL_POSITION, position[0], position[1], position[2]);
}
void Audio_SourceSetVelocity(u32 source, vec3 velocity) {
  alSource3f(source, AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
}
