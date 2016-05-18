#include "platform.h"

#if __has_builtin(__builtin_trap)
#define Assert(Expression) if(!(Expression)) { __builtin_trap(); }
#endif

#include "platform.h"
#include <dlfcn.h>
#include <sys/stat.h>
#include "osx_main.h"
#include <stdio.h>
#include <cstdlib>
#include <cassert>
#include <sys/stat.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

FREE_FILE(psFreeFile)
{
    if(memory)
    {
        free(memory);
    }
}

READ_ENTIRE_FILE(psReadEntireFile)
{
    // do this later
    read_file result = {};
    FILE *entireFile = fopen(filePath, "rb");
    if(entireFile)
    {
        size_t fileSize;
        fseek(entireFile, 0L, SEEK_END);
        fileSize = ftell(entireFile);
        rewind(entireFile);
        result.memory = (void *) malloc(fileSize);
        result.size = fread(result.memory, sizeof(uint8), fileSize, entireFile);
    }
    return result;
}

int32
main(int32 argc, char **argv)
{
    printf("welcome\n");
    if(!glfwInit())
    {
        printf("failed to initialize glfw\n");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Junk", nullptr, nullptr);
    if(window == nullptr)
    {
        printf("Failed to create window via GLFW\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glViewport(0, 0, 800, 600);

    const char *dyLibPath = "libgame.so";
    OSXDyLib gameCode = osxLoadGameCode(dyLibPath);
    thread_context thread = {};
    GameMemory memory = {};
    memory.isInitialized = false;
    memory.platformServiceReadFile = psReadEntireFile;
    memory.permanentStorageSize = Megabytes(256);
    memory.transientStorageSize = Gigabytes(1);
    uint64 totalSize = memory.permanentStorageSize + memory.transientStorageSize;

    void *allTheDamnedMemory = malloc(totalSize);
    printf("allocated the memory\n");
    memory.permStorage = allTheDamnedMemory;
    memory.transStorage = ((uint8 *)allTheDamnedMemory + memory.permanentStorageSize);

    real64 timeVal;
    
    while(!glfwWindowShouldClose(window))
    {
        bool32 gameCodeReloaded = false;
        if(hasDyLibUpdatedSinceLastRead(gameCode, dyLibPath))
        {
            gameCode = osxLoadGameCode(dyLibPath);
            gameCodeReloaded = true;
        }
        glfwPollEvents();
        timeVal = glfwGetTime();
        GameInput inputForFrame = { false };
        if(gameCode.gameUpdate)
        {
            gameCode.gameUpdate(&thread, &memory, &inputForFrame, timeVal, gameCodeReloaded);
        }
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
