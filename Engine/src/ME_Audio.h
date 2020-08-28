#ifndef _ME_AUDIO_H
#define _ME_AUDIO_H

#include "types.h"
#include <SDL2/SDL.h>

typedef struct ME_Audio
{
    SDL_AudioSpec wavSpec;
    u32 wavLength;
    u8 *wavBuffer;
    SDL_AudioDeviceID deviceId;
    
} ME_Audio;

ME_Audio ME_LoadAudio(const char *fileName);
void ME_PlayAudio(ME_Audio *audio);
void ME_FreeAudio(ME_Audio *audio);

#endif //_ME_AUDIO_H
