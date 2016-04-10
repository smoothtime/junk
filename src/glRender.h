#if !defined(GLRENDER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.cpp"

Vertex hardcodedVerts[] =
{
    //Pos                 TextCoords
    {  0.5f,  0.5f, 0.0f, 1.0f, 1.0f },
    {  0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
    { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f },
    { -0.5f,  0.5f, 0.0f, 0.0f, 1.0f }
};

Vertex hardcodedVerts2[] =
{
    //Pos                Color               TextCoords
    {  0.5f,  1.5f, -5.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f },
    {  0.5f,  0.5f, -5.0f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f },
    { -0.5f,  0.5f, -5.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f },
    { -0.5f,  1.5f, -5.0f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f }
};

GLuint hardcodedIndices[] =
{
    0, 1, 3,
    1, 2, 3
};

struct RenderReferences {
    uint32 numObjects;
    uint32 maxObjects;
    Shader *shaders;
    GLuint *textures;
    GLuint *VAOs;
    GLuint *VBOs;
    GLuint *EBOs;
    mat4 *modelMatrices;
};

void
initShaders(RenderReferences *rendRefs)
{
    for(uint32 r = 0; r < rendRefs->numObjects; ++r)
    {
        char vShaderName[FILENAME_MAX];
        char fShaderName[FILENAME_MAX];
        sprintf_s(vShaderName, "../data/vshader_%d.vs", r + 1);
        sprintf_s(fShaderName, "../data/fshader_%d.fs", r + 1);
        rendRefs->shaders[r] = Shader(vShaderName, fShaderName);
    }
}

void
initTextures(RenderReferences *rendRefs)
{
    int32 x1, y1, n1, x2, y2, n2;
    uint8 *imageData1= stbi_load("../data/wall.jpg", &x1, &y1, &n1, 0);
    uint8 *imageData2= stbi_load("../data/other.png", &x2, &y2, &n2, 0);

    glGenTextures(2, rendRefs->textures);
    for(uint32 r = 0; r < rendRefs->numObjects; ++r)
    {
        glBindTexture(GL_TEXTURE_2D, rendRefs->textures[r]);
        // Specify options and stuff
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Yup
        if(r == 0)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x1, y1, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData1);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x2, y2, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData2);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData1);
    stbi_image_free(imageData2);
}

void
initObjects(RenderReferences *rendRefs)
{
    for(uint32 i = 0; i < rendRefs->numObjects; ++i)
    {
        // Create objects
        glGenVertexArrays(1, rendRefs->VAOs + i);
        glGenBuffers(1, rendRefs->VBOs + i);
        glGenBuffers(1, rendRefs->EBOs + i);

        // bind to objects to store state
        glBindVertexArray(rendRefs->VAOs[i]);
        glBindBuffer(GL_ARRAY_BUFFER, rendRefs->VBOs[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendRefs->EBOs[i]);

        // copy data into GPU buffer where final parameter determines what sort of spot it's put in
        glBufferData(GL_ARRAY_BUFFER, sizeof(hardcodedVerts), hardcodedVerts, GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(hardcodedVerts2), hardcodedVerts2, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hardcodedIndices), hardcodedIndices, GL_STATIC_DRAW);
        // NOTE(james): parameters mean:
        // which attribute, how many elements, data type of each element,
        // should  we normalize (cap between -1 and 1) data, space between consecutive attributes (stride),
        // offset to where data begins in buffer
        // NOTE(james): this call is also what bind the VBO to the VAO, so you can unbind that without issue
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) 0);
        // NOTE(james): parameter is location of vertAttrib
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *) (6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        
        // Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}

#define GLRENDER_H
#endif
