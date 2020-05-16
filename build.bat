@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall" x64

mkdir obj
pushd obj
cl /c ..\src\MicroEngine\MicroEngine.c /I..\include\MicroEngine /I..\SDL2\include
cl /c ..\src\MicroEngine\ME_GameObject.c /I..\include\MicroEngine /I..\SDL2\include
cl /c ..\src\MicroEngine\ME_ParticleSystem.c /I..\include\MicroEngine /I..\SDL2\include
cl /c ..\src\MicroEngine\ME_Vector2D.c /I..\include\MicroEngine /I..\SDL2\include
cl /c ..\src\MicroEngine\ME_Utility.c /I..\include\MicroEngine /I..\SDL2\include

cl /c ..\src\MicroUI\MicroUI.c /I..\include\MicroUI /I..\SDL2\include
cl /c ..\src\MicroUI\MUI_Button.c /I..\include\MicroUI /I..\SDL2\include
cl /c ..\src\MicroUI\MUI_CheckBox.c /I..\include\MicroUI /I..\SDL2\include
cl /c ..\src\MicroUI\MUI_Slider.c /I..\include\MicroUI /I..\SDL2\include
cl /c ..\src\MicroUI\MUI_TextBox.c /I..\include\MicroUI /I..\SDL2\include
popd

if not exist bin mkdir bin
pushd bin
cl ..\src\Pong.c /I..\include\MicroEngine /I..\include\MicroUI /I..\SDL2\include /link /SUBSYSTEM:CONSOLE ^
            /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib
cl ..\src\FlappyBird_game.c /I..\include\MicroEngine /I..\include\MicroUI /I..\SDL2\include /link /SUBSYSTEM:CONSOLE ^
            /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib
cl ..\src\MicroUI_test.c /I..\include\MicroEngine /I..\include\MicroUI /I..\SDL2\include /link /SUBSYSTEM:CONSOLE ^
            /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib
cl ..\src\MicroEngine_test.c /I..\include\MicroEngine /I..\include\MicroUI /I..\SDL2\include /link /SUBSYSTEM:CONSOLE ^
            /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib
cl ..\src\SpaceShip_Game.c /I..\include\MicroEngine /I..\include\MicroUI /I..\SDL2\include /link /SUBSYSTEM:CONSOLE ^
            /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib
cl ..\src\Vector_Field.c /I..\include\MicroEngine /I..\include\MicroUI /I..\SDL2\include /link /SUBSYSTEM:CONSOLE ^
            /LIBPATH:..\SDL2\lib\x64 ..\obj\*.obj SDL2.lib SDL2main.lib SDL2_image.lib SDL2_ttf.lib Shell32.lib
popd bin




