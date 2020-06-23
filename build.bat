@echo off
rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x64

set lflags= SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib user32.lib Kernel32.lib

set sdl2_include= /I..\SDL2\include  

if not exist bin mkdir bin
pushd bin

rem pong
REM cl /nologo ..\game\Pong.c /I..\src %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%
rem Pong.exe

rem flappybird
REM cl /nologo ..\game\FlappyBird_game.c %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags% 
rem FlappyBird_game.exe

rem ui_test
del MicroUI_test.exe
cl /nologo ..\game\MicroUI_test.c /I..\src  %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%
MicroUI_test.exe

rem engine_test
cl /nologo ..\game\MicroEngine_test.c /I..\src %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%

rem spaceship 
REM cl /nologo ..\game\SpaceShip_Game.c /I..\src %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%

rem vectorfield
REM cl /nologo ..\game\Vector_Field.c /I..\src %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%

popd bin