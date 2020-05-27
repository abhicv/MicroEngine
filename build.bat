@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x64

set lflags=SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib

set microengine_include=%microengine_include%
set microui_include=/I..\include\MicroUI
set sdl2_include=/I..\SDL2\include

set microengine_src=..\src\MicroEngine\
set microui_src=..\src\MicroUI\  

if not exist obj mkdir obj
pushd obj
cl /c %microengine_include% %sdl2_include% ^
..\src\MicroEngine\MicroEngine.c ^
..\src\MicroEngine\ME_GameObject.c ^
..\src\MicroEngine\ME_ParticleSystem.c ^
..\src\MicroEngine\ME_Vector2D.c ^
..\src\MicroEngine\ME_Utility.c 

cl /c %microui_include% %sdl2_include% ^
..\src\MicroUI\MicroUI.c ^
..\src\MicroUI\MUI_Button.c ^
..\src\MicroUI\MUI_CheckBox.c ^
 ..\src\MicroUI\MUI_Slider.c ^
..\src\MicroUI\MUI_TextBox.c
popd

if not exist bin mkdir bin
pushd bin
cl ..\src\Pong.c %microengine_include% %microui_include% %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj %lflags%

cl ..\src\FlappyBird_game.c %microengine_include% %microui_include% %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj %lflags%

cl ..\src\MicroUI_test.c %microengine_include% %microui_include% %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj %lflags%

cl ..\src\MicroEngine_test.c %microengine_include% %microui_include% %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj %lflags%

cl ..\src\SpaceShip_Game.c %microengine_include% %microui_include% %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj %lflags%

cl ..\src\Vector_Field.c %microengine_include% %microui_include% %sdl2_include% ^
/link /SUBSYSTEM:CONSOLE /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj %lflags%
popd bin




