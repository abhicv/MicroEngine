@echo off
rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

set microengine_src= /I..\Engine\src
set sdl2_include= /I..\SDL2\include
set sdl2_lib= /libpath:..\SDL2\lib\x64

set lflags= SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib

if not exist ..\bin mkdir ..\bin
pushd ..\bin

rem game code dll
cl /Zi /nologo /LD ..\GameZero\gamezero.c %sdl2_include% %microengine_src% /link /nodefaultlib:msvcrt.lib %sdl2_lib% %lflags%

rem platform dependent executable
if exist sdl2_gamezero.c del sdl2_gamezero.c

cl /Zi /nologo ..\GameZero\sdl2_gamezero.c %sdl2_include% /link /nodefaultlib:msvcrt.lib /incremental:no /subsystem:console %sdl2_lib% %lflags% gamezero.lib

popd ..\bin