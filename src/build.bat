@echo off
set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4100 -wd4189 -wd4201 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ..\src\win32_main.cpp -Fmwin32_main.map /link %CommonLinkerFlags%
popd
