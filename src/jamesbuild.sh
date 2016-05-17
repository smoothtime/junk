#!/bin/sh
COMMON_COMPILER_FLAGS="-I../src/include"
COMMON_LINKER_FLAGS=""
OPENGL_LINKER_FLAGS=""
if [ ! -d ../build ] mkdir ../build
pushd ../build
clang++ -std=c++11 -g -shared -I../src/include -L../lib/ -o libgame.so ../src/game.cpp -lGLEW.2.0.0 -framework OpenGL
clang++ -std=c++11 -g -I../src/include -L../lib ../src/osx_main.cpp -framework OpenGL -lglfw.3.2 -lGLEW.2.0.0 
popd
