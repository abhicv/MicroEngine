#ifndef MUI_SLIDER_H
#define MUI_SLIDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    SDL_Rect borderRect;
    SDL_Rect slidingRect;
    SDL_Rect sliderHandleRect;

    float value;
    float maxValue;
    float minValue;

} MUI_Slider;

MUI_Slider MUI_CreateSlider(int x, int y, float currentValue, float maxValue, float minValue);

bool MUI_OnSliderValueChange(MUI_Slider *slider, SDL_Event event);

int MUI_GetSliderValue(MUI_Slider *slider);

void MUI_RenderSlider(MUI_Slider *slider, SDL_Renderer *renderer);

void MUI_DestroySlider(MUI_Slider *slider);

#endif // MUI_SLIDER_H
