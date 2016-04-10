/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */

#include <windows.h>
#include "platform.h"
#include "win32_main.h"
#include <stdio.h>
#include <cstdlib>
#include <cassert>
#include "radixsort.h"

#include "Allocator.h"
#include "MallocAllocator.h"
#include "GlobalAllocator.h"

// TODO(james): get rid of these
#include "Vec3.cpp"
#include "matrix.h"
#include <GLFW/glfw3.h>

void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

READ_RENDERABLE(psReadRenderable)
{
    // I'll do this later
    read_file result = {};
    return result;
}

int CALLBACK
WinMain(HINSTANCE instance,
        HINSTANCE prevInstance,
        LPSTR commandLine,
        int showCode)
{

    char platformExecutableName[WIN32_FILE_NAME_SIZE];
    DWORD platExeNameSize = GetModuleFileName(0, platformExecutableName, sizeof(platformExecutableName));
    uint32 onePastLastSlash = 0;
    for(uint32 i = platExeNameSize - 1; i > 0; --i)
    {
        if(platformExecutableName[i] == '\\')
        {
            onePastLastSlash = i+1;
            break;
        }
    }
    char gameDLLPath[WIN32_FILE_NAME_SIZE];
    strncpy_s(gameDLLPath, platformExecutableName, onePastLastSlash);
    strcat_s(gameDLLPath, "game.dll");

    char tempDLLPath[WIN32_FILE_NAME_SIZE];
    strncpy_s(tempDLLPath, platformExecutableName, onePastLastSlash);
    strcat_s(tempDLLPath, "game_temp.dll");

    char dllLockFilePath[WIN32_FILE_NAME_SIZE];
    strncpy_s(dllLockFilePath, platformExecutableName, onePastLastSlash);
    strcat_s(dllLockFilePath, "game.lock");

    Win32GameDLL gameDLL = win32LoadGameCode(gameDLLPath, tempDLLPath, dllLockFilePath);    


    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Junk", nullptr, nullptr);
    if(window == nullptr)
    {
        printf("Failed to create window via GLFW");
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glViewport(0, 0, 800, 600);

    thread_context thread = {};
    GameMemory memory = {};
    memory.isSimulationInitialized = false;
    memory.isRendererInitialized = false;
    memory.platformServiceReadRenderable = psReadRenderable;
    memory.permanentStorageSize = Megabytes(256);
    memory.transientStorageSize = Gigabytes(1);
    uint64 totalSize = memory.permanentStorageSize + memory.transientStorageSize;

    //NOTE(james): this is all copied from HH stuff and I'm not sure about all the TODO:'s listed beloew
    // TODO(casey): Handle various memory footprints (USING
    // SYSTEM METRICS)

    // TODO(casey): Use MEM_LARGE_PAGES and
    // call adjust token privileges when not on Windows XP?

    // TODO(casey): TransientStorage needs to be broken up
    // into game transient and cache transient, and only the
    // former need be saved for state playback.
    void *allTheDamnedMemory = VirtualAlloc(0, (size_t) totalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    memory.permStorage = allTheDamnedMemory;
    memory.transStorage = ((uint8 *)allTheDamnedMemory + memory.permanentStorageSize);

    real64 timeVal;

    while(!glfwWindowShouldClose(window))
    {
        FILETIME newDLLWriteTime = win32GetLastWriteTime(gameDLLPath);
        if(CompareFileTime(&newDLLWriteTime, &gameDLL.dllLastWriteTime) != 0)
        {
            win32UnloadGameCode(&gameDLL);
            gameDLL = win32LoadGameCode(gameDLLPath, tempDLLPath, dllLockFilePath);
            break;
        }
        glfwPollEvents();
        timeVal = glfwGetTime();
        GameInput inputForFrame = { false };
        if(gameDLL.gameUpdate && gameDLL.gameRender)
        {
            gameDLL.gameUpdate(&thread, &memory, &inputForFrame, timeVal);
            gameDLL.gameRender(&thread, &memory, timeVal);
        }
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
    
