#ifndef MUI_CHECKBOX_H
#define MUI_CHECKBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdbool.h>

typedef struct
{
    bool checked;
    SDL_Texture *bgTexture;
    SDL_Texture *tickTexture;
    SDL_Rect rect;
    bool enabled;

} MUI_CheckBox;

MUI_CheckBox MUI_CreateCheckBox(int, int);
bool MUI_CheckBoxEvent(MUI_CheckBox*, SDL_Event);
void MUI_RenderCheckBox(MUI_CheckBox*, SDL_Renderer *renderer);
void MUI_DestroyCheckBox(MUI_CheckBox*);

#endif // MUI_CHECKBOX_H
