@echo off
emcc -s ASM_JS=1 -O2 --preload-file ..\bin\data sdl2_gamezero_em.c -o gamezero.html -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=["png"] 