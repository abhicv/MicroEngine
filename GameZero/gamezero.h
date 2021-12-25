#ifndef GAMEZERO_H
#define GAMEZERO_H

#include <SDL2/SDL.h>
#include "..\Engine\src\types.h"

#define LOAD_DATA(name) void name(SDL_Renderer *renderer)
#define HANDLE_EVENT(name) void name(SDL_Event *event)
#define GAME_UPDATE_AND_RENDER(name) void name(u32 *gameMode, bool *quitGame,f32 deltaTime, SDL_Renderer *renderer)
#define SHOW_SCREEN(name) void name(u32 *gameMode, SDL_Event *event, SDL_Renderer *renderer)

//used by the sdl platform  layer to run the game
extern __declspec(dllexport) LOAD_DATA(LoadData);
extern __declspec(dllexport) HANDLE_EVENT(HandleEvent);
extern __declspec(dllexport) GAME_UPDATE_AND_RENDER(GameUpdateAndRender);
extern __declspec(dllexport) SHOW_SCREEN(ShowMainMenu);
extern __declspec(dllexport) SHOW_SCREEN(ShowCredits);

//dll export to test game levels from level editor
extern __declspec(dllexport) void LoadLevelData(const char *entityMapFile, const char *tileMapFile,
                                                const char *collisionMapFile, SDL_Renderer *renderer);
enum GameMode
{
    GAME_MAIN_MENU = 0,
    GAME_CREDITS,
    GAME_LOAD_LEVEL,
    GAME_PLAY,
    GAME_LEVEL_MENU,
    GAME_EXIT,
};

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 704

#endif //GAMEZERO_H
