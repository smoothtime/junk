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

#include <GLFW/glfw3.h>

GameInput inputLastFrame;
GameInput inputForFrame;

void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        inputForFrame.wantsToTerminate = true;
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(key == GLFW_KEY_W)
    {
        if(action == GLFW_PRESS)
            inputForFrame.w = true;
        else if(action == GLFW_RELEASE)
            inputForFrame.w = false;
    }
    if(key == GLFW_KEY_A)
    {
        if(action == GLFW_PRESS)
            inputForFrame.a = true;
        else if(action == GLFW_RELEASE)
            inputForFrame.a = false;
    }
    if(key == GLFW_KEY_S)
    {
        if(action == GLFW_PRESS)
            inputForFrame.s = true;
        else if(action == GLFW_RELEASE)
            inputForFrame.s = false;
    }
    if(key == GLFW_KEY_D)
    {
        if(action == GLFW_PRESS)
            inputForFrame.d = true;
        else if(action == GLFW_RELEASE)
            inputForFrame.d = false;
    }
     
}

bool mouseInit = true;
void mouse_callback(GLFWwindow *window, real64 xpos, real64 ypos)
{
    OutputDebugStringA("Hit mouse cb\n");
    if(mouseInit)
    {
        inputForFrame.mouseX = (real32) xpos;
        inputForFrame.mouseY = (real32) ypos;
        inputForFrame.mouseDeltaX = 0;
        inputForFrame.mouseDeltaY = 0;
        mouseInit = false;
        return;
    }

    inputForFrame.mouseX = (real32) xpos;
    inputForFrame.mouseY = (real32) ypos;
    inputForFrame.mouseDeltaX = (real32) xpos - inputLastFrame.mouseX;
    inputForFrame.mouseDeltaY = inputLastFrame.mouseY - (real32) ypos;
    
}

bool32 focused = false;
void focus_callback(GLFWwindow *window, int32 focus)
{
    focused = focus;
}

FREE_FILE(psFreeFile)
{
    if(memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
    }
}

READ_ENTIRE_FILE(psReadEntireFile)
{
    // I'll do this later
    read_file result = {};
    HANDLE fileHandle = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if(GetFileSizeEx(fileHandle, &fileSize))
        {
            uint32 fileSize32 = (uint32) fileSize.QuadPart;
            result.memory = VirtualAlloc(0, fileSize32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(result.memory)
            {
                DWORD bytesRead;
                if(ReadFile(fileHandle, result.memory, fileSize32, &bytesRead, 0)
                   && fileSize32 == bytesRead)
                {
                    result.size = bytesRead;
                }
                else
                {
                    psFreeFile(thread, result.memory);
                    result.memory = 0;
                }
            }
        }
        CloseHandle(fileHandle);
    }
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetWindowFocusCallback(window, focus_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glViewport(0, 0, 800, 600);

    thread_context thread = {};
    GameMemory memory = {};
    memory.isInitialized = false;
    memory.platformServiceReadFile = psReadEntireFile;
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

    real64 lastTime = 0;
    real64 deltaTime;

    while(!glfwWindowShouldClose(window))
    {
        FILETIME newDLLWriteTime = win32GetLastWriteTime(gameDLLPath);
        bool32 gameCodeReloaded = false;
        if(CompareFileTime(&newDLLWriteTime, &gameDLL.dllLastWriteTime) != 0)
        {
            win32UnloadGameCode(&gameDLL);
            gameDLL = win32LoadGameCode(gameDLLPath, tempDLLPath, dllLockFilePath);
            gameCodeReloaded = true;
        }
        glfwPollEvents();
        real64 time = glfwGetTime();
        deltaTime  = time - lastTime;
        lastTime = time;

        char output[256];
        _snprintf_s(output, sizeof(output),
                    "MouseX for frame: %f.\n MouseY for frame: %f.\n DeltaX for frame: %f.\n DeltaY for frame: %f.\n", inputForFrame.mouseX, inputForFrame.mouseY, inputForFrame.mouseDeltaX, inputForFrame.mouseDeltaY);
        OutputDebugStringA(output);
        if(gameDLL.gameUpdate)
        {
            gameDLL.gameUpdate(&thread, &memory, &inputForFrame, deltaTime, gameCodeReloaded);
        }
        inputLastFrame = inputForFrame;
        inputForFrame.mouseDeltaX = 0;
        inputForFrame.mouseDeltaY = 0;

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
    
