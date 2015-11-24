@echo off
set CommonCompilerFlags=-MT -nologo /I ..\src\include -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4100 -wd4189 -wd4201 -wd4577 -wd4505 -wd4127 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib ..\lib\glfw3dll.lib opengl32.lib ..\lib\glew32s.lib

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags%  ..\src\win32_main.cpp -Fmwin32_main.map /link %CommonLinkerFlags%
popd
