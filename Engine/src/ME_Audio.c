#include "ME_Audio.h"

ME_Audio ME_LoadAudio(const char *fileName)
{
    ME_Audio audio = {0};
    SDL_LoadWAV(fileName, &audio.wavSpec, &audio.wavBuffer, &audio.wavLength);
    audio.deviceId = SDL_OpenAudioDevice(NULL, 0, &audio.wavSpec, NULL, 0);
    return audio;
}

void ME_PlayAudio(ME_Audio *audio)
{
    u32 success = SDL_QueueAudio(audio->deviceId, audio->wavBuffer, audio->wavLength);
    if(success == 0)
    {
        SDL_PauseAudioDevice(audio->deviceId, 0);
    }
}

void ME_FreeAudio(ME_Audio *audio)
{
    SDL_CloseAudioDevice(audio->deviceId);
    SDL_FreeWAV(audio->wavBuffer);
    audio->wavBuffer = NULL;
}
