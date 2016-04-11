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

struct RenderReferences {
    uint32 numObjects;
    uint32 maxObjects;
    Shader *shaders;
    GLuint *textures;
    GLuint *VAOs;
    GLuint *VBOs;
    GLuint *EBOs;
    glm::mat4 *modelMatrices;
};

void
initShader(RenderReferences *rendRefs, char *vShaderPath, char *fShaderPath)
{
    assert(rendRefs->numObjects <  rendRefs->maxObjects -1);
    rendRefs->shaders[rendRefs->numObjects] = Shader(vShaderPath,  fShaderPath);
}

// TODO(james): make the texture properties passed in
void
initTexture(RenderReferences *rendRefs, char *texturePath)
{
    int32 x, y, n;
    uint8 *imageData= stbi_load(texturePath, &x, &y, &n, 0);

    glGenTextures(1, rendRefs->textures);
    glBindTexture(GL_TEXTURE_2D, rendRefs->textures[0]);
    // Specify options and stuff
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);
}

//TODO(james): make it so you can pass in parameters like GL_STATIC_DRAW, etc
void
initObject(RenderReferences *rendRefs, Model *model)
{
    
    glGenVertexArrays(1, rendRefs->VAOs + rendRefs->numObjects);
    glGenBuffers(1, rendRefs->VBOs + rendRefs->numObjects);
    glGenBuffers(1, rendRefs->EBOs + rendRefs->numObjects);

    glBindVertexArray(rendRefs->VAOs[rendRefs->numObjects]);
    
    glBindBuffer(GL_ARRAY_BUFFER, rendRefs->VBOs[rendRefs->numObjects]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * model->numVerts, model->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendRefs->EBOs[rendRefs->numObjects]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * model->numIndices, model->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

#if 0

void
initObjects(RenderReferences *rendRefs)
{
    // Create objects
    glGenVertexArrays(1, rendRefs->VAOs);
    glGenBuffers(1, rendRefs->VBOs);
    glGenBuffers(1, rendRefs->EBOs);

    // bind to objects to store state
    glBindVertexArray(*rendRefs->VAOs);
    glBindBuffer(GL_ARRAY_BUFFER, *rendRefs->VBOs);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *rendRefs->EBOs);

    // copy data into GPU buffer where final parameter determines what sort of spot it's put in
    glBufferData(GL_ARRAY_BUFFER, sizeof(hardcodedVerts), hardcodedVerts, GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(hardcodedVerts2), hardcodedVerts2, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hardcodedIndices), hardcodedIndices, GL_STATIC_DRAW);
    // NOTE(james): parameters mean:
    // which attribute, how many elements, data type of each element,
    // should  we normalize (cap between -1 and 1) data, space between consecutive attributes (stride),
    // offset to where data begins in buffer
    // NOTE(james): this call is also what binds the VBO to the VAO, so you can unbind that without issue
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) 0);
    // NOTE(james): parameter is location of vertAttrib
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
        
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void
initTextures(RenderReferences *rendRefs)
{
    int32 x1, y1, n1, x2, y2, n2;
    uint8 *imageData1= stbi_load("../data/wall.jpg", &x1, &y1, &n1, 0);
    uint8 *imageData2= stbi_load("../data/other.png", &x2, &y2, &n2, 0);

    glGenTextures(1, rendRefs->textures);

    glBindTexture(GL_TEXTURE_2D, rendRefs->textures[0]);
    // Specify options and stuff
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x1, y1, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData1);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData1);
    stbi_image_free(imageData2);
}


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
#endif

#define GLRENDER_H
#endif
