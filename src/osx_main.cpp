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

    OSXDyLib gameCode = osxLoadGameCode("/Users/jwells/Development/cpp/junk/build/libgame.so");
    if(gameCode.gameUpdate)
    {
        printf("loaded right\n");
    }
    else
    {
        printf("something's fucked\n"); 
    }
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
