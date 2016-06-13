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
    uint32 maxObjects;
    uint32 numShaders;
    uint32 numTextures;
    uint32 numVAOs;
    uint32 numVBOs;
    uint32 numEBOs;
    Shader *shaders;
    GLuint *textures;
    GLuint *VAOs;
    GLuint *VBOs;
    GLuint *EBOs;
};

struct RenderReferenceIndex {
    uint32 shaderIndex;
    uint32 textureIndex;
    uint32 VAOIndex;
    uint32 VBOIndex;
    uint32 EBOIndex;
    uint32 numIndices;
};

void
initShader(RenderReferences *rendRefs, const char *vShaderPath, const char *fShaderPath)
{
    assert(rendRefs->numShaders <  rendRefs->maxObjects -1);
    rendRefs->shaders[rendRefs->numShaders] = Shader(vShaderPath,  fShaderPath);
    rendRefs->numShaders++;
}

// TODO(james): make the texture properties passed in
void
initTexture(RenderReferences *rendRefs, const char *texturePath)
{
    int32 x, y, n;
    uint8 *imageData= stbi_load(texturePath, &x, &y, &n, 0);

    glGenTextures(1, rendRefs->textures);
    glBindTexture(GL_TEXTURE_2D, rendRefs->textures[rendRefs->numTextures]);
    // Specify options and stuff
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imageData);

    rendRefs->numTextures++;
}

//TODO(james): make it so you can pass in parameters like GL_STATIC_DRAW, etc
void
initVertexIndexBuffers(RenderReferences *rendRefs, Mesh *mesh)
{
    
    glGenVertexArrays(1, rendRefs->VAOs + rendRefs->numVAOs);
    glGenBuffers(1, rendRefs->VBOs + rendRefs->numVBOs);
    glGenBuffers(1, rendRefs->EBOs + rendRefs->numEBOs);

    glBindVertexArray(rendRefs->VAOs[rendRefs->numVAOs]);
    
    glBindBuffer(GL_ARRAY_BUFFER, rendRefs->VBOs[rendRefs->numVBOs]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->numVerts, mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendRefs->EBOs[rendRefs->numEBOs]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * mesh->numIndices, mesh->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    rendRefs->numVAOs++;
    rendRefs->numVBOs++;
    rendRefs->numEBOs++;
}

void
overrideVertexBuffers(RenderReferences *rendRefs, RenderReferenceIndex toClear, Mesh* model)
{
    glBindVertexArray(rendRefs->VAOs[rendRefs->numVAOs]);
    
    glBindBuffer(GL_ARRAY_BUFFER, rendRefs->VBOs[toClear.VBOIndex]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * model->numVerts, model->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendRefs->EBOs[toClear.EBOIndex]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * model->numIndices, model->indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}
#define GLRENDER_H
#endif
