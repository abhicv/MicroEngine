@echo off
REM call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x64

set lflags= SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib
set sdl2_include= /I..\..\SDL2\include  
set sdl2_lib= /LIBPATH:..\..\SDL2\lib\x64

if not exist bin mkdir bin
pushd bin

REM pong
REM if exist Pong.exe del Pong.exe
REM cl /nologo ..\game\Pong.c /I..\src %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%
REM Pong.exe

REM flappybird
REM del FlappyBird_game.exe
REM cl /nologo ..\game\FlappyBird_game.c %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags% 
REM FlappyBird_game.exe

REM ui_test
REM if exist MicroUI_test.exe del MicroUI_test.exe
REM cl /nologo ..\game\MicroUI_test.c /I..\src  %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%
REM if exist MicroUI_test.exe MicroUI_test.exe

REM engine_test
REM cl /nologo ..\game\MicroEngine_test.c /I..\src %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%

REM micro_ecs_test
if exist MicroECS_test.exe del MicroECS_test.exe    
cl /Zi /nologo ..\game\MicroECS_test.c /I..\src %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE %sdl2_lib%  %lflags%
if exist MicroECS_test.exe MicroECS_test.exe

REM spaceship
REM del SpaceShip_Game.exe 
REM cl /nologo ..\game\SpaceShip_Game.c /I..\src %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%
REM SpaceShip_Game.exe

REM vectorfield
REM cl /nologo ..\game\Vector_Field.c /I..\src %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%

REM micro_editor
REM if exist MicroEditor.exe del MicroEditor.exe    
REM cl /nologo ..\game\MicroEditor.c /I..\src %sdl2_include% ^
REM /link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 %lflags%
REM if exist MicroEditor.exe MicroEditor.exe

popd bin