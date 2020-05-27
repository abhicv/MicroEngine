#ifndef MUI_BUTTON_H
#define MUI_BUTTON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "MUI_TextBox.h"

#define DEFAULT_BUTTON_WIDTH 100
#define DEFAULT_BUTTON_HEIGHT 30
#define DEFAULT_BUTTON_FONT_SIZE 20

typedef enum {

    CLICKED,
    ACTIVE,
    INACTIVE,

} MUI_ButtonState;

typedef struct {
    SDL_Texture *bgTexture;
    SDL_Rect rect;
    MUI_TextBox label;
    bool enabled;
    MUI_ButtonState state;
    SDL_Color highlightColor;
    SDL_Color normalColor;

} MUI_Button;

MUI_Button MUI_CreateButton(int x, int y);

bool MUI_ButtonPressed(MUI_Button *button, SDL_Event event);

void MUI_SetButtonPosition(MUI_Button *button, int x, int y);

void MUI_SetButtonLabel(MUI_Button *button, const char *label);

void MUI_SetButtonLabelFont(MUI_Button *button, TTF_Font *font);

void MUI_RenderButton(MUI_Button *button, SDL_Renderer *renderer);

void MUI_DestroyButton(MUI_Button *button);


#endif //MUI_BUTTON_H
