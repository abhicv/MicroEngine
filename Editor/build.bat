@echo off
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x64

set microengine_src= /I..\Engine\src
set sdl2_include= /I..\SDL2\include
set sdl2_lib= /LIBPATH:..\SDL2\lib\x64

set lflags= SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib

if not exist ..\bin mkdir ..\bin
pushd ..\bin

if exist MicroEditor.exe del MicroEditor.exe
cl /nologo /O2 ..\Editor\MicroEditor.c %microengine_src% %sdl2_include% /link /incremental:no /subsystem:console %sdl2_lib% %lflags%
if exist MicroEditor.exe MicroEditor.exe

popd ..\bin