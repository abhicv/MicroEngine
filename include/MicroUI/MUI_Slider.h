#ifndef MUI_SLIDER_H
#define MUI_SLIDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    SDL_Rect rect;
    int value;

} MUI_Slider;

MUI_Slider MUI_CreateSlider();
bool OnSliderValueChange(MUI_Slider *slider);
void MUI_RenderSlider(SDL_Renderer *renderer, MUI_Slider *slider);
void MUI_DestroySlider(MUI_Slider *slider);

#endif // MUI_SLIDER_H
