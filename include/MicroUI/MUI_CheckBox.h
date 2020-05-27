#ifndef MUI_CHECKBOX_H
#define MUI_CHECKBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    SDL_Texture *bgTexture;
    SDL_Texture *tickTexture;
    SDL_Rect rect;
    bool checked;
    bool enabled;

} MUI_CheckBox;

MUI_CheckBox MUI_CreateCheckBox(int x, int y);

bool MUI_CheckBoxEvent(MUI_CheckBox *checkBox, SDL_Event event);

void MUI_RenderCheckBox(MUI_CheckBox *checkBox, SDL_Renderer *renderer);

void MUI_DestroyCheckBox(MUI_CheckBox *checkBox);

#endif // MUI_CHECKBOX_H
