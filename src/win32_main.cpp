/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include <windows.h>
#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;
typedef uintptr_t uptr;

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <cassert>
#include "allocator.h"
#include "mallocallocator.h"
#include "globalallocator.h"
#include "jset.h"
#include "jvector.h"
#include "Vec3.h"
#include "Entity.h"
#include "Octree.cpp"
#include "radixsort.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>



static junk::JVector<Entity> entities(GlobalAllocator());
GLfloat hardcodedVerts[] =
{
    // First triangle
     0.5f,  0.5f, 0.0f,  // Top Right
     0.5f, -0.5f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, 0.0f,  // Bottom Left
    -0.5f,  0.5f, 0.0f  // Top Left      
};

GLuint hardcodedIndices[] =
{
    0, 1, 3,
    1, 2, 3
};

const GLchar *hardcodedVShader1 =
"#version 330 core \n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
    "gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\n\0";
const GLchar *hardcodedVShader2 =
"#version 330 core \n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
    "gl_Position = vec4(position.x - 0.3, position.y, position.z, 2.0);\n"
"}\n\0";

const GLchar *hardcodedFShader1 =
"#version 330 core \n"
"out vec4 color;\n"
"void main()\n"
"{\n"
    "color = vec4(1.0f, 0.5f, 0.2f, 1.0f);"
"}\n\0";
const GLchar *hardcodedFShader2 =
"#version 330 core \n"
"out vec4 color;\n"
"void main()\n"
"{\n"
    "color = vec4(0.2f, 0.5f, 1.0f, 1.0f);"
"}\n\0";

const GLchar *hardcodedVShaders[2] = {
    hardcodedVShader1,
    hardcodedVShader2
};

const GLchar *hardcodedFShaders[2] = {
    hardcodedFShader1,
    hardcodedFShader2
};

struct RenderReferences {
    uint32 numObjects;
    GLuint *shaderPrograms;
    GLuint *VAOs;
    GLuint *VBOs;
    GLuint *EBOs;
};

RenderReferences rendRefs;

void
JVectorTest(Allocator *mallocator)
{
    junk::JVector<int32> *test1 = new junk::JVector<int32>(mallocator);
    test1->push_back(4);
    test1->push_back(2);
    test1->push_back(1);
    test1->push_back(0);
    test1->push_back(-1);
    int32 four = (*test1)[0];
    int32 first = *test1->begin();
    int32 last = *test1->end();
    size_t length = test1->size();
    test1->resize(length + 1);
    test1->resize(4);
    test1->resize(16, -7);
}

void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void
gameLogic()
{
    MallocAllocator mallocator;

    Vec3 vec1(-0.5f, -0.5f, -0.5f);
    Vec3 vec2( 0.5f,  0.5f,  0.5f);
    Vec3 origin = vec1 + ((vec2 - vec1) * 0.5f);

    AABBox box1(vec1, vec2);
    AABBox box2(0.01f, 0.01f, 0.01f, 0.05f, 0.05f, 0.05f); 
    
    uint32 entityIds[5] = {0};
    uint32 entityCount = 0;

    entities.push_back(Entity(1, box2));

    entities.push_back(Entity(2, AABBox(box2._min * 2.0f, box2._max * 2.0)));
    entities.push_back(Entity(3, AABBox(box2._min * 3.0f, box2._max * 3.0)));
    entities.push_back(Entity(4, AABBox(box2._min * 4.0f, box2._max * 4.0)));
    entities.push_back(Entity(5, AABBox(box2._min * 5.0f, box2._max * 5.0)));

    Octree tree(&mallocator, origin, AABBox(vec1, vec2), &entities);
    for(uint32 i = 0; i < entities.size(); ++i)
    {
        tree.insert(i);
    }
}

void inline
checkShaderCompilation(GLuint shader)
{
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Failed to compile shader:\n%s\n", infoLog);
    }
}

void inline
checkShaderProgramCompilation(GLuint program)
{
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        printf("Failed to compile shader program:\n%s\n", infoLog);
    }
}

void
initShaders()
{
    rendRefs.shaderPrograms = (GLuint *) GlobalAllocator()->allocate(sizeof(GLuint) * rendRefs.numObjects, sizeof(GLuint));
    for(uint32 r = 0; r < rendRefs.numObjects; ++r)
    {
        GLuint vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &hardcodedVShaders[r], NULL);
        glCompileShader(vertexShader);
        checkShaderCompilation(vertexShader);

        GLuint fragmentShader;
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &hardcodedFShaders[r], NULL);
        glCompileShader(fragmentShader);
        checkShaderCompilation(fragmentShader);

        rendRefs.shaderPrograms[r] = glCreateProgram();
        glAttachShader(rendRefs.shaderPrograms[r], vertexShader);
        glAttachShader(rendRefs.shaderPrograms[r], fragmentShader);
        glLinkProgram(rendRefs.shaderPrograms[r]);
        checkShaderProgramCompilation(rendRefs.shaderPrograms[r]);
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

}

void
initObjects()
{

    rendRefs.VAOs = (GLuint *) GlobalAllocator()->allocate(sizeof(GLuint) * rendRefs.numObjects, sizeof(GLuint));
    rendRefs.VBOs = (GLuint *) GlobalAllocator()->allocate(sizeof(GLuint) * rendRefs.numObjects, sizeof(GLuint));
    rendRefs.EBOs = (GLuint *) GlobalAllocator()->allocate(sizeof(GLuint) * rendRefs.numObjects, sizeof(GLuint));

    for(uint32 i = 0; i < rendRefs.numObjects; ++i)
    {
        // Create objects
        glGenVertexArrays(1, rendRefs.VAOs + i);
        glGenBuffers(1, rendRefs.VBOs + i);
        glGenBuffers(1, rendRefs.EBOs + i);

        // bind to objects to store state
        glBindVertexArray(rendRefs.VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, rendRefs.VBOs[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendRefs.EBOs[i]);

        // copy data into GPU buffer where final parameter determines what sort of spot it's put in
        glBufferData(GL_ARRAY_BUFFER, sizeof(hardcodedVerts), hardcodedVerts, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hardcodedIndices), hardcodedIndices, GL_STATIC_DRAW);
        // NOTE(james): parameters mean:
        // which attribute, how many elements, data type of each element,
        // should  we normalize (cap between -1 and 1) data, space between consecutive attributes (stride),
        // offset to where data begins in buffer
        // NOTE(james): this call is also what bind the VBO to the VAO, so you can unbind that without issue
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
        // NOTE(james): parameter is location of vertAttrib
        glEnableVertexAttribArray(0);

        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}

void
render()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for(uint32 r = 0; r < rendRefs.numObjects; ++r)
    {
        glUseProgram(rendRefs.shaderPrograms[r]);
        glBindVertexArray(rendRefs.VAOs[r]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

}

void
doOpenGLStuff()
{
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

    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK)
    {
        printf("Failed to init GLEW");
    }

    glfwSetKeyCallback(window, key_callback);

    glViewport(0, 0, 800, 600);

    rendRefs.numObjects = 2;
    initShaders();
    initObjects();

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        //       gameLogic();
        render();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}

int CALLBACK
WinMain(HINSTANCE instance,
        HINSTANCE prevInstance,
        LPSTR commandLine,
        int showCode)
{
    doOpenGLStuff();    
}
    
