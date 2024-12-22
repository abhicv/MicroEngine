#include "MicroEditor.h"
#include "File.c"
#include "Render.c"
#include "util.c"

#include "style.h"

#include "..\Engine\src\MicroEngine.c"
#include "GamePlayView.c"
#include "EditorView.c"

// const u32 displayWidth = 1920;
// const u32 displayHeight = 1080;

const u32 displayWidth = 1600;
const u32 displayHeight = 900;

#define MUI_ORIGIN_ID 2023

void MainMenuLoop(SDL_Renderer *renderer, u32 *appMode)
{
    MUI ui = {0};
    ui.fontFile = "assets/Font/OCRAEXT.ttf";

    bool quit = false;
    
    u32 mode = MENU_MODE;
    
    TextEdit tmapFilePath = {0};
    TextEdit cmapFilePath = {0};
    TextEdit emapFilePath = {0};
    
    TextEdit tileMapWidthText = {0};
    TextEdit tileMapHeightText = {0};
    TextEdit tileWidthText =  {0};
    TextEdit tileHeightText = {0};
    TextEdit tileSheetTileWidthText = {0};
    TextEdit tileSheetTileHeightText = {0};
    
    b32 fileNamesFound = true;

    char *levelFilePaths[10];
    int totalFilesFound = 0;    
    GetLevelFileList(levelFilePaths, &totalFilesFound);

    SDL_Event event = {0};

    MUI_Input uiInput = {0};
    
    while(!quit)
    {
        uiInput.bTextInput = false;
        uiInput.backSpaceDown = false;
        
        while(SDL_PollEvent(&event) > 0)
        {
            MUI_GetInput(&uiInput, &event);
            
            switch(event.type)
            {
                case SDL_QUIT:
                quit = true;
                (*appMode) = APP_MODE_QUIT;
                break;
                
                case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quit = true;
                    (*appMode) = APP_MODE_QUIT;
                }
                break;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 18, 18, 18, 0);
        SDL_RenderClear(renderer);
        
        MUI_BeginFrame(&ui, &uiInput);
        
        MUI_PushColumnLayout(&ui, (MUI_Rect){10, 10, 250, 35}, 10);
        {
            if(mode == MENU_MODE)
            {
                MUI_TextA(&ui, GEN_MUI_ID(), "MICRO EDITOR by @bhicv", 40, textStyle, MUI_TEXT_ALIGN_LEFT);
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "create level", buttonStyle))
                {
                    mode = CREATE_LEVEL_MODE;
                }
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "load level", buttonStyle))
                {
                    mode = LOAD_LEVEL_MODE;
                }

                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "load default level", buttonStyle))
                {
                    mode = LOAD_DEFAULT_LEVEL;
                }
            }
            
            if(mode == CREATE_LEVEL_MODE)
            {
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 15);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Map Width:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileMapWidthText);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Map Height:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileMapHeightText);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 15);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Tile Width:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileWidthText);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Tile Height:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileHeightText);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 15);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Sheet Tile Width:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileSheetTileWidthText);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "Sheet Tile Height:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_TextEditA(&ui, GEN_MUI_ID(), buttonStyle, &tileSheetTileHeightText);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 15);
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "back", buttonStyle))
                {
                    mode = MENU_MODE;
                }
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "create", buttonStyle))
                {
                    u32 tileMapWidth = StringToInteger(tileMapWidthText.text, strlen(tileMapWidthText.text));
                    u32 tileMapHeight = StringToInteger(tileMapHeightText.text, strlen(tileMapHeightText.text));
                    u32 tileWidth = StringToInteger(tileWidthText.text, strlen(tileWidthText.text));
                    u32 tileHeight = StringToInteger(tileHeightText.text, strlen(tileHeightText.text));
                    u32 tileSheetTileWidth = StringToInteger(tileSheetTileWidthText.text, strlen(tileSheetTileWidthText.text));
                    u32 tileSheetTileHeight = StringToInteger(tileSheetTileHeightText.text, strlen(tileSheetTileHeightText.text));
                    
                    tileMap = ME_CreateTileMap(tileMapWidth, tileMapHeight, tileWidth, tileHeight, tileSheetTileWidth, tileSheetTileHeight);
                    
                    quit = true;
                    (*appMode) = APP_MODE_EDITOR;
                    
                    InitializeEditorData(renderer, displayWidth);
                }
                
                MUI_PopLayout(&ui);                
            }
            else if(mode == LOAD_LEVEL_MODE)
            {
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 10);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "tile map:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                MUI_Rect r = MUI_GetNextAutoLayoutRect(&ui);
                r.width = 1000;
                MUI_TextEdit(&ui, GEN_MUI_ID(), r, buttonStyle, &tmapFilePath);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 10);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "collision map:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                r = MUI_GetNextAutoLayoutRect(&ui);
                r.width = 1000;
                MUI_TextEdit(&ui, GEN_MUI_ID(), r, buttonStyle, &cmapFilePath);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 10);
                
                MUI_TextA(&ui, GEN_MUI_ID(), "entity map:", 20, textStyle, MUI_TEXT_ALIGN_RIGHT);
                r = MUI_GetNextAutoLayoutRect(&ui);
                r.width = 1000;
                MUI_TextEdit(&ui, GEN_MUI_ID(), r, buttonStyle, &emapFilePath);
                
                MUI_PopLayout(&ui);
                
                MUI_PushRowLayout(&ui, MUI_GetNextAutoLayoutRect(&ui), 10);
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "back", buttonStyle))
                {
                    mode = MENU_MODE;
                    fileNamesFound = true;
                }
                
                if(MUI_ButtonA(&ui, GEN_MUI_ID(), "load", buttonStyle))
                {
                    if(strlen(tmapFilePath.text) > 0)
                    {
                        tileMap = ME_LoadTileMap(tmapFilePath.text);
                    }
                    else
                    {
                        fileNamesFound = false;
                    }
                    
                    if(strlen(cmapFilePath.text) > 0)
                    {
                        collisionSpace = LoadCollisionMap(cmapFilePath.text);
                    }
                    else
                    {
                        fileNamesFound = false;
                    }
                    
                    if(strlen(emapFilePath.text) > 0)
                    {
                        entitySpace = LoadEntityMap(emapFilePath.text);
                    }
                    else
                    {
                        fileNamesFound = false;
                    }
                    
                    if(fileNamesFound)
                    {
                        quit = true;
                        (*appMode) = APP_MODE_EDITOR;
                        InitializeEditorData(renderer, displayWidth);
                    }
                }
                MUI_PopLayout(&ui);
                
                Color red = {255, 0, 0 ,255};
                MUI_Style warning = {
                    .textStyle = {
                        .textColor = red
                    }
                };
                
                if(strlen(tmapFilePath.text) == 0 && !fileNamesFound)
                {
                    MUI_TextA(&ui, GEN_MUI_ID(), "> tile map file name missing !", 20, warning, MUI_TEXT_ALIGN_LEFT);
                }
                
                if(strlen(cmapFilePath.text) == 0 && !fileNamesFound)
                {
                    MUI_TextA(&ui, GEN_MUI_ID(), "> collision map file name missing !", 20, warning, MUI_TEXT_ALIGN_LEFT);
                }
                
                if(strlen(emapFilePath.text) == 0 && !fileNamesFound)
                {
                    MUI_TextA(&ui, GEN_MUI_ID(), "> entity map file name missing !", 20, warning, MUI_TEXT_ALIGN_LEFT);
                }

                MUI_TextA(&ui, GEN_MUI_ID(), "Levels", 20, textStyle, MUI_TEXT_ALIGN_LEFT);
                for(int n = 0; n < totalFilesFound; n++) 
                {
                    if(MUI_ButtonA(&ui, GEN_MUI_IDi(n), levelFilePaths[n], textOnlybuttonStyle)) 
                    {
                        printf("Loading level '%s' ....\n", levelFilePaths[n]);
                    }
                }
            }
            else if(mode == LOAD_DEFAULT_LEVEL)
            {
                tileMap = ME_LoadTileMap(tileMapFileName);
                collisionSpace = LoadCollisionMap(collisionMapFileName);
                entitySpace = LoadEntityMap(entityMapFileName);
                quit = true;
                (*appMode) = APP_MODE_EDITOR;
                InitializeEditorData(renderer, displayWidth);
            }
        }

        MUI_PopLayout(&ui);
        
        MUI_EndFrame(&ui, renderer);
        
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char *argv[])
{    
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to initialize SDL : %s\n", SDL_GetError());
        return 1;
    }
    
    if(TTF_Init() == -1)
    {
        SDL_Log("Failed to initialize TTF : %s\n", TTF_GetError());
    }
    
    if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
    {
        SDL_Log("Failed to initialize IMG : %s\n", IMG_GetError());
    }

    SDL_Window *window = SDL_CreateWindow("MicroEditor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              displayWidth, displayHeight, 0);
    
    if(window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n", SDL_GetError());
        return 1;
    }
    
    u32 appMode = APP_MODE_MAIN_MENU;
    
    bool quitApp = false;
    while(!quitApp)
    {
        switch(appMode)
        {
            case APP_MODE_EDITOR:
            EditorLoop(renderer, &appMode);
            break;
            
            case APP_MODE_GAME_TEST:
            GameTestLoop(renderer, &appMode);
            break;
            
            case APP_MODE_MAIN_MENU:
            MainMenuLoop(renderer, &appMode);
            break;
            
            case APP_MODE_QUIT:
            quitApp = true;
            break;
        }
    }
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}