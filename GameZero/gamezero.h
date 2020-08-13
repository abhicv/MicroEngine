#ifndef GAMEZERO_H
#define GAMEZERO_H

#include <SDL2/SDL.h>
#include "..\Engine\src\types.h"

#define LOAD_DATA(name) void name(SDL_Renderer *renderer)
#define HANDLE_EVENT(name) void name(SDL_Event *event)
#define GAME_UPDATE_AND_RENDER(name) void name(f32 deltaTime,SDL_Renderer *renderer)
#define SHOW_SCREEN(name) void name(u32 *gameCode, SDL_Event *event, SDL_Renderer *renderer)

typedef LOAD_DATA(loadData);
typedef HANDLE_EVENT(handleEvent);
typedef GAME_UPDATE_AND_RENDER(gameUpdateAndRender);
typedef SHOW_SCREEN(showMainMenu);
typedef SHOW_SCREEN(showCredits);

LOAD_DATA(LoadDataStub){}
HANDLE_EVENT(HandleEventStub){}
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub){}
SHOW_SCREEN(ShowMainMenuStub){}
SHOW_SCREEN(ShowCreditsStub){}

__declspec(dllexport) LOAD_DATA(LoadData);
__declspec(dllexport) HANDLE_EVENT(HandleEvent);
__declspec(dllexport) GAME_UPDATE_AND_RENDER(GameUpdateAndRender);
__declspec(dllexport) SHOW_SCREEN(ShowMainMenu);
__declspec(dllexport) SHOW_SCREEN(ShowCredits);

enum GameMode
{
    GAME_MAIN_MENU,
    GAME_CREDITS,
    GAME_PLAY,
    GAME_EXIT,
};

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 704

#endif //GAMEZERO_H
