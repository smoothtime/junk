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
    uint32 colorPickFBO;
    uint32 colorPickTexture;
    uint32 colorPickDepthRenderBuffer;
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

uint32
initShader(RenderReferences *rendRefs, const char *vShaderPath, const char *fShaderPath)
{
    assert(rendRefs->numShaders <  rendRefs->maxObjects -1);
    rendRefs->shaders[rendRefs->numShaders] = Shader(vShaderPath,  fShaderPath);

    return rendRefs->numShaders++;
}

// TODO(james): make the texture properties passed in
uint32
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

    return rendRefs->numTextures++;
}

//TODO(james): make it so you can pass in parameters like GL_STATIC_DRAW, etc
uint32
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

    assert(rendRefs->numVAOs == rendRefs->numVBOs);
    assert(rendRefs->numVAOs == rendRefs->numEBOs);

    return rendRefs->numVAOs - 1;
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

void renderEntities(GameState *gameState, glm::mat4 glmView, glm::mat4 glmProjection)
{
    glm::mat4 glmModel;
    RenderReferences *rr = gameState->rendRefs;
    for(uint32 x = 0;
            x < gameState->entityCount;
            x++)
        {
            Entity *entity = gameState->dynamicEntities + x;
            glmModel = entity->transMtx * entity->rotMtx;
            for(uint32 m = 0;
                m < entity->model->numRenderMesh;
                ++m)
            {

                RenderMesh *rm = entity->model->renderMeshes + m;
                GLuint shaderProgram = rr->shaders[rm->rri.shaderIndex].program;
                glUseProgram(shaderProgram);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, rr->textures[rm->rri.textureIndex]);
                glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture1"), 0);

                GLint modelMatrix = glGetUniformLocation(shaderProgram, "model");
                GLint viewMatrix = glGetUniformLocation(shaderProgram, "view");
                GLint projMatrix = glGetUniformLocation(shaderProgram, "projection");
        
                glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(glmModel));
                glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(glmView));
                glUniformMatrix4fv(projMatrix, 1, GL_FALSE, glm::value_ptr(glmProjection));
            
                glBindVertexArray(rr->VAOs[rm->rri.VAOIndex]);
                glDrawElements(GL_TRIANGLES, rm->rri.numIndices, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                //        glEnable(GL_BLEND);
                //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                //        glDisable(GL_BLEND);
                glUseProgram(0);
            }

        }
}

void checkGLError(platformLog* log)
{
    char blep[256];
    GLenum x = glGetError();
    sproot(blep, "the error %d", x);
    log(blep);
}

#define GLRENDER_H
#endif
