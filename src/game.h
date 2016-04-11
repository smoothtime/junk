#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "platform.h"
#include "Entity.h"
#include "Octree.cpp"
#include "glRender.h"

struct Frustum
{
    real32 n;
    real32 f;
    real32 l;
    real32 r;
    real32 t;
    real32 b;
};

struct Camera
{
    glm::vec3 position;
    Frustum frustum;
    glm::mat4 viewMatrix;
};

struct GameState
{
    MemoryArena memArena;
    Entity staticEntities[10000];
    uint32 entityCount;
    Octree *staticEntityTree;
    Camera camera;
    glm::vec3 testDelta;
    uint32 numModels;
    Model *models;
    uint32 maxModels;    
    RenderReferences *rendRefs;

    GLfloat *vertices;
    GLuint *indices;
};

void
loadModel(thread_context *thread, GameState *gameState, platformServiceReadEntireFile *psRF, char *relPath)
{
    read_file loadedModel = psRF(thread, relPath);
    // Parse file


    // Hacking to test
    //Model model = {};
    Model *model = gameState->models + gameState->numModels;
    model->numVerts = 4;
    model->vertices = PushArray(&gameState->memArena, 4, Vertex);
    model->vertices[0].pos = glm::vec3( 1.0f,  0.5f,  0.0f);
    model->vertices[1].pos = glm::vec3( 0.5f, -0.5f,  0.0f);
    model->vertices[2].pos = glm::vec3(-0.5f, -0.5f,  0.0f);
    model->vertices[3].pos = glm::vec3(-1.0f,  0.5f,  0.0f);
    model->vertices[0].texCoords = glm::vec2(1.5f, 1.0f);
    model->vertices[1].texCoords = glm::vec2(1.0f, 0.0f);
    model->vertices[2].texCoords = glm::vec2(0.0f, 0.0f);
    model->vertices[3].texCoords = glm::vec2(-0.5f, 1.0f);
    model->numIndices = 6;
    model->indices = PushArray(&gameState->memArena, 6, uint32);
    model->indices[0] = 0;
    model->indices[1] = 1;
    model->indices[2] = 3;
    model->indices[3] = 1;
    model->indices[4] = 2;
    model->indices[5] = 3;
    
    initShader(gameState->rendRefs, "../data/vshader_1.vs", "../data/fshader_1.fs");
    initTexture(gameState->rendRefs, "../data/wall.jpg");
    initObject(gameState->rendRefs, model);
    // </hacking>

    gameState->numModels++;
    gameState->rendRefs->numObjects++;
    
}

#define GAME_H
#endif
