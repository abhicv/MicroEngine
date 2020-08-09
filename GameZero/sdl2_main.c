#include "gamezero.c"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 704

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event = {0};
	
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Failed to initialize SDL : %s\n", SDL_GetError());
        return 1;
    }
	
    if (TTF_Init() == -1)
    {
        SDL_Log("Failed to initialize TTF : %s\n", TTF_GetError());
    }
	
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
    {
        SDL_Log("Failed to initialize IMG : %s\n", IMG_GetError());
    }
	
    window = SDL_CreateWindow("GameZero",
							  SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED,
							  SCREEN_WIDTH,
							  SCREEN_HEIGHT,
							  SDL_WINDOW_ALLOW_HIGHDPI);
	
    if (window == NULL)
    {
        SDL_Log("Failed to create SDL window : %s\n", SDL_GetError());
        return 1;
    }
	
    renderer = SDL_CreateRenderer(window,
								  3,
								  SDL_RENDERER_ACCELERATED |
								  SDL_RENDERER_PRESENTVSYNC);
	
    if (renderer == NULL)
    {
        SDL_Log("Failed to create SDL renderer : %s\n", SDL_GetError());
        return 1;
    }
	
    f32 deltaTime = 0.016f;
    u64 startTime = 0;
    u64 endTime = 0;
    bool quitGame = false;
	
    LoadData(renderer);
	
    while(!quitGame)
    {
        startTime = SDL_GetPerformanceCounter();
		
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
				case SDL_QUIT:
                quitGame = true;
                break;
				
				case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    quitGame = true;
                }
                else if(event.key.keysym.sym == SDLK_r)
                {
                    LoadData(renderer);
                }
                break;
                
            }
			HandleEvent(&event);
        }
		
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
		
        GameUpdateAndRender(deltaTime, renderer);
		
        SDL_RenderPresent(renderer);
		
        endTime = SDL_GetPerformanceCounter();
        u64 elapsed = (endTime - startTime) * 1000 / SDL_GetPerformanceFrequency();
        deltaTime = (float)elapsed / 1000.0f;
		
        if(deltaTime > 0.016f)
        {
            deltaTime = 0.016f;
        }
    }
	
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
	
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
	
    return 0;
}