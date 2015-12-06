/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include "win32_main.h"
#include <windows.h>
#include "platform.h"
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

#include "Shader.cpp"

static junk::JVector<Entity> entities(GlobalAllocator());
GLfloat hardcodedVerts[] =
{
     0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 
     0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f
};

GLuint hardcodedIndices[] =
{
    0, 1, 3,
    1, 2, 3
};

struct RenderReferences {
    uint32 numObjects;
    Shader *shaders;
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

void
initShaders()
{
    rendRefs.shaders = (Shader *) GlobalAllocator()->allocate(sizeof(Shader) * rendRefs.numObjects, NULL);
    for(uint32 r = 0; r < rendRefs.numObjects; ++r)
    {
        char vShaderName[FILENAME_MAX];
        char fShaderName[FILENAME_MAX];
        sprintf_s(vShaderName, "../data/vshader_%d.vs", r + 1);
        sprintf_s(fShaderName, "../data/fshader_%d.fs", r + 1);
        rendRefs.shaders[r] = Shader(vShaderName, fShaderName);
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *) 0);
        // NOTE(james): parameter is location of vertAttrib
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

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
        rendRefs.shaders[r].use();
        
        GLdouble timeVal = glfwGetTime();
        GLdouble sinTime = (sin(timeVal)) + 0.5f;
        GLint uniColorLoc = glGetUniformLocation(rendRefs.shaders[r].program, "uniColor");
        glUniform4f(uniColorLoc, 0.0f, (GLfloat) sinTime, 0.0f, 1.0f);
            
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
    
