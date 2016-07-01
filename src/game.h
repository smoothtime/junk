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
    uint32 entityCount;
    Octree *staticEntityTree;
    Camera camera;
    uint32 maxModels;
    uint32 numModels;
    Model *models;
    RenderReferences *rendRefs;
    uint32 resWidth, resHeight;

    real64 deltaTime;
};

struct readMeshListEntry
{
    Mesh **entry;
    readMeshListEntry *next;
};

Model *
loadModel(thread_context *thread, GameState *gameState,
          platformServiceReadEntireFile *psRF, const char *relPath,
          bool32 isCollisionMesh)
{
    const uint16 CHUNK_MAIN                = 0x4d4d;
    const uint16 CHUNK_3D_EDITOR           = 0x3d3d;
    const uint16 CHUNK_OBJECT_BLOCK        = 0x4000;
    const uint16 CHUNK_TRIANGULAR_MESH     = 0x4100;
    const uint16 CHUNK_VERTICES_LIST       = 0x4110;
    const uint16 CHUNK_FACE_LIST           = 0x4120;
    const uint16 CHUNK_MAPPING_COORDINATES = 0x4140;
    const uint16 CHUNK_MATERIAL_BLOCK      = 0xafff;

    GeneralAllocator *alctr = gameState->assetAlctr;
    Model *result = gameState->models + gameState->numModels;
    
    uint32 meshCount = 0;
    readMeshListEntry *first = NEW(alctr, readMeshListEntry);
    readMeshListEntry *current = first;

    Mesh *currentMesh = NEW(alctr, Mesh);
    current->entry = &currentMesh;
    
    read_file loadedFile = psRF(thread, relPath);
    // Parse file
    uint16 *chunkId;
    uint32 *chunkLength;
    uint16 *count;
    uint16 *faceFlags;
    uint8 *readP = (uint8 *)loadedFile.memory;
    while(readP < (uint8 *)loadedFile.memory + loadedFile.size)
    {
        chunkId = (uint16 *)readP;
        readP += sizeof(uint16);
        chunkLength = (uint32 *)readP;
        readP += sizeof(uint32);
        switch(*chunkId)
        {
            case CHUNK_MAIN:
                // read children
                break;
                
            case CHUNK_3D_EDITOR:
                // read children
                break;
                
            case CHUNK_OBJECT_BLOCK:
            {
                JString *name = readString(&gameState->memArena, readP);
                readP += name->size;
                break;
            }
            
            case CHUNK_TRIANGULAR_MESH:
                // read children
                if(meshCount++ > 0)
                {
                    current->next = NEW(alctr, readMeshListEntry);
                    current = current->next;
                    currentMesh = NEW(alctr, Mesh);
                    current->entry = &currentMesh; 
                }
                break;
                
            case CHUNK_VERTICES_LIST:
            {
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                currentMesh->numVerts = (uint32) *count;
                currentMesh->vertices = (Vertex *) gameState->assetAlctr->alloc(sizeof(Vertex) * currentMesh->numVerts);
                for(int32 i = 0; i < *count; ++i)
                {
                    currentMesh->vertices[i].pos = glm::vec3( *((real32 *)readP + 0),
                                                        *((real32 *)readP + 1),
                                                        *((real32 *)readP + 2)
                                                      );
                    readP += 3 * sizeof(real32);
                }
                break;
            }
            
            case CHUNK_FACE_LIST:
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                currentMesh->numIndices = (uint32)(*count * 3);
                currentMesh->indices = (uint32 *) gameState->assetAlctr->alloc(sizeof(uint32) * currentMesh->numIndices);
                for(uint32 i = 0; i < currentMesh->numIndices; i+=3)
                {
                    currentMesh->indices[i + 0] = (uint32)*((uint16 *)readP);
                    currentMesh->indices[i + 1] = (uint32)*((uint16 *)readP + 1);
                    currentMesh->indices[i + 2] = (uint32)*((uint16 *)readP + 2);
                    // pass a not important face flag
                    faceFlags = (uint16 *) readP;
                    readP += 4 * sizeof(uint16);
                }
                break;
                 
            case CHUNK_MAPPING_COORDINATES:
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                for(uint32 i = 0; i < currentMesh->numVerts; ++i)
                {
                    currentMesh->vertices[i].texCoords = glm::vec2( *((real32 *)readP + 0),
                                                              *((real32 *)readP + 1)
                                                           );
                    readP += 2 * sizeof(real32);
                }
                break;
                
            case CHUNK_MATERIAL_BLOCK:
                // postponed until we get all the verts displaying correctly
                readP += (*chunkLength - 6); // -6 for the header we've read already
                break;
                
            default:
                readP += (*chunkLength - 6); // -6 for the header we've read already
        }
    }

    if(!isCollisionMesh)
    {
        result->numRenderMesh = meshCount;
        result->renderMeshes = (RenderMesh *) alctr->alloc(sizeof(Mesh) * meshCount);
        current = first;
        for(uint32 i = 0; i < meshCount; ++i)
        {
            RenderMesh *renderMesh = result->renderMeshes + i;
            renderMesh->mesh = *(current->entry);
            readMeshListEntry *tmp = current->next;
            alctr->dealloc(current);
            current = tmp;
        }
        assert(current == 0);

    }
    else
    {
        
        result->numCollisionMesh = meshCount;
        result->collisionMeshes = (CollisionMeshPair *) alctr->alloc(sizeof(CollisionMeshPair) * meshCount);
        current = first;
        for(uint32 i = 0; i < meshCount; ++i)
        {
            result->collisionMeshes[i].baseMesh = *(current->entry);
            Mesh *bm = result->collisionMeshes[i].baseMesh;
            readMeshListEntry *tmp = current->next;
            alctr->dealloc(current);
            current = tmp;

            result->collisionMeshes[i].worldMesh = NEW(alctr, Mesh);
            Mesh *wm = result->collisionMeshes[i].worldMesh;
            wm->numVerts = bm->numVerts;
            wm->numIndices = bm->numIndices;
            wm->vertices = (Vertex *) alctr->alloc(sizeof(Vertex) * bm->numVerts);
            wm->indices = (uint32 *) alctr->alloc(sizeof(uint32) * bm->numIndices);
        }
        assert(current == 0);
    }
    
#define RETARDED_CUBE_3DS 0
#if RETARDED_CUBE_3DS
    for(uint32 i = 0; i < currentMesh->numIndices; ++i)
    {
        switch(currentMesh->indices[i])
        {
            case 0:
            case 1:
                currentMesh->indices[i] = 0;
                break;
            case 2:
            case 3:
            case 4:
                currentMesh->indices[i] = 2;
                break;
            case 5:
            case 6:
            case 7:
                currentMesh->indices[i] = 5;
                break;
            case 8:
            case 9:
            case 10:
                currentMesh->indices[i] = 8;
                break;
            case 11:
                currentMesh->indices[i] = 11;
                break;
            case 12:
            case 13:
            case 14:
                currentMesh->indices[i] = 12;
                break;
            case 15:
            case 16:
            case 17:
                currentMesh->indices[i] = 15;
                break;
            case 18:
            case 19:
                currentMesh->indices[i] = 18;
                break;
            default:
                break;
                
        }
    }
#endif
        
    return result;
}

#define GAME_H
#endif
