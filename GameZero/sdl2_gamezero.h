#ifndef SDL2_GAMEZERO_H
#define SDL2_GAMEZERO_H

#include "gamezero.h"

global SDL_Window *window = 0;
global SDL_Renderer *renderer = 0;
global SDL_Event event = {0};
global bool quitGlobal = false;
global u32 gameMode = GAME_MAIN_MENU;

#endif //SDL2_GAMEZERO_H
