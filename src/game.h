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
    Model *models;
    uint32 maxModels;    
    RenderReferences *rendRefs;
};

void
LoadModel(thread_context *thread, GameState *gameState, platformServiceReadEntireFile *psRF, char *relPath)
{
    read_file loadedModel = psRF(thread, relPath);
    // Parse file
    Model model = {};

    // Hacking to test
    model.numVerts = 4;
    model.vertices[0].pos = glm::vec3( 0.5f,  0.5f,  0.0f);
    model.vertices[1].pos = glm::vec3( 0.5f, -0.5f,  0.0f);
    model.vertices[2].pos = glm::vec3(-0.5f, -0.5f,  0.0f);
    model.vertices[3].pos = glm::vec3(-0.5f,  0.5f,  0.0f);
    model.vertices[0].texCoords = glm::vec2(1.0f, 1.0f);
    model.vertices[1].texCoords = glm::vec2(1.0f, 0.0f);
    model.vertices[2].texCoords = glm::vec2(0.0f, 0.0f);
    model.vertices[3].texCoords = glm::vec2(0.0f, 1.0f);

    initShader(gameState->rendRefs, "../data/vshader_1.vs", "../data/fshader_1.fs");
    initTexture(gameState->rendRefs, "../data/wall.jpg");
    initObject(gameState->rendRefs, &model);
    // </hacking>

    // Add to models stored in GameState
}

#define GAME_H
#endif
