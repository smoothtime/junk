#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "platform.h"
#include "GeneralAllocator.cpp"
#include "Model.h"
#include "Entity.cpp"
#include "GJK.h"
#include "Octree.cpp"
#include "BucketArray.cpp"

glm::vec3 WORLD_UP_VECTOR = glm::vec3(0.0f, 1.0f, 0.0f);

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
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;
    real32 pitch;
    real32 yaw;
};

Camera
initCamera(glm::vec3 pos, glm::vec3 dir, glm::vec3 worldUp)
{
    Camera result = {};
    result.position = pos;
    result.direction = glm::normalize(dir);
    glm::vec3 tar = pos + dir;
    result.right = glm::normalize(glm::cross(worldUp, glm::normalize(pos - tar)));;
    result.up = glm::normalize(glm::cross(glm::normalize(pos - tar), result.right));
    result.pitch = 0.0f;
    result.yaw = PI32 / -2.0f;
    return result;
}

struct RenderReferences;
struct GameState
{
    MemoryArena memArena;
    GeneralAllocator *assetAlctr;
    Entity dynamicEntities[10000];
    IDSystem *entity_ids;
    uint32 entityCount;
    Octree *staticEntityTree;
    Camera camera;
    uint32 maxModels;
    uint32 numModels;
    Model *models;
    RenderReferences *rendRefs;
    uint32 resWidth, resHeight;

    bool32 isLevelStarted;
    uint32 gameLevel;
    real64 deltaTime;
};

struct readMeshListEntry
{
    Mesh **entry;
    readMeshListEntry *next;
};

#define GAME_H
#endif
