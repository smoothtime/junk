@echo off
set CommonCompilerFlags=-MT -nologo /I ..\src\include -fp:fast -Gm- -EHa- -GR- -Od -Oi -WX -W4 -wd4100 -wd4189 -wd4201 -wd4577 -wd4505 -wd4127 -wd4530 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib ..\lib\glfw3dll.lib 
set OpenGLLinkerFlags= opengl32.lib ..\lib\glew32s.lib

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

REM 64-bit build
del *.pdb > NUL 2> NUL
REM Optimization switches /O2
echo WAITING FOR PDB > lock.tmp
cl %CommonCompilerFlags% ..\src\game.cpp -Fmgame.map -LD /link -incremental:no -opt:ref %OpenGLLinkerFlags%  -PDB:game_%random%.pdb -EXPORT:gameUpdate
del lock.tmp
REM James migrate from GLFW so you don't pair the rendering with the platform
cl %CommonCompilerFlags%  ..\src\win32_main.cpp -Fmwin32_main.map /link %CommonLinkerFlags% %OpenGLLinkerFlags%
popd
