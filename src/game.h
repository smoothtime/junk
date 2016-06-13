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
#include "glRender.h"
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

struct GameState
{
    MemoryArena memArena;
    GeneralAllocator *assetAlctr;
    Entity staticEntities[10000];
    uint32 entityCount;
    Octree *staticEntityTree;
    Camera camera;
    uint32 numModels;
    Model *models;
    uint32 maxModels;    
    RenderReferences *rendRefs;

    real64 deltaTime;
};

Model *
loadModel(thread_context *thread, GameState *gameState, platformServiceReadEntireFile *psRF, const char *relPath)
{
    const uint16 CHUNK_MAIN                = 0x4d4d;
    const uint16 CHUNK_3D_EDITOR           = 0x3d3d;
    const uint16 CHUNK_OBJECT_BLOCK        = 0x4000;
    const uint16 CHUNK_TRIANGULAR_MESH     = 0x4100;
    const uint16 CHUNK_VERTICES_LIST       = 0x4110;
    const uint16 CHUNK_FACE_LIST           = 0x4120;
    const uint16 CHUNK_MAPPING_COORDINATES = 0x4140;
    const uint16 CHUNK_MATERIAL_BLOCK      = 0xafff;

    Model *model = gameState->models + gameState->numModels;
    read_file loadedMesh = psRF(thread, relPath);
    // Parse file
    uint16 *chunkId;
    uint32 *chunkLength;
    uint16 *count;
    uint16 *faceFlags;
    uint8 *readP = (uint8 *)loadedMesh.memory;
    while(readP < (uint8 *)loadedMesh.memory + loadedMesh.size)
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
                break;
                
            case CHUNK_VERTICES_LIST:
            {
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                model->baseMesh.numVerts = (uint32) *count;
                model->baseMesh.vertices = (Vertex *) gameState->assetAlctr->alloc(sizeof(Vertex) * model->baseMesh.numVerts);
                for(int32 i = 0; i < *count; ++i)
                {
                    model->baseMesh.vertices[i].pos = glm::vec3( *((real32 *)readP + 0),
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
                model->baseMesh.numIndices = (uint32)(*count * 3);
                model->baseMesh.indices = (uint32 *) gameState->assetAlctr->alloc(sizeof(uint32) * model->baseMesh.numIndices);
                for(uint32 i = 0; i < model->baseMesh.numIndices; i+=3)
                {
                    model->baseMesh.indices[i + 0] = (uint32)*((uint16 *)readP);
                    model->baseMesh.indices[i + 1] = (uint32)*((uint16 *)readP + 1);
                    model->baseMesh.indices[i + 2] = (uint32)*((uint16 *)readP + 2);
                    // pass a not important face flag
                    faceFlags = (uint16 *) readP;
                    readP += 4 * sizeof(uint16);
                }
                break;
                 
            case CHUNK_MAPPING_COORDINATES:
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                for(uint32 i = 0; i < model->baseMesh.numVerts; ++i)
                {
                    model->baseMesh.vertices[i].texCoords = glm::vec2( *((real32 *)readP + 0),
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
    model->worldMesh.numVerts = model->baseMesh.numVerts;
    model->worldMesh.numIndices = model->baseMesh.numIndices;
    model->worldMesh.vertices = (Vertex *) gameState->assetAlctr->alloc(sizeof(Vertex) * model->baseMesh.numVerts);
    model->worldMesh.indices = (uint32 *) gameState->assetAlctr->alloc(sizeof(uint32) * model->baseMesh.numIndices);

#define RETARDED_CUBE_3DS 0
#if RETARDED_CUBE_3DS
    for(uint32 i = 0; i < model->baseMesh.numIndices; ++i)
    {
        switch(model->baseMesh.indices[i])
        {
            case 0:
            case 1:
                model->baseMesh.indices[i] = 0;
                break;
            case 2:
            case 3:
            case 4:
                model->baseMesh.indices[i] = 2;
                break;
            case 5:
            case 6:
            case 7:
                model->baseMesh.indices[i] = 5;
                break;
            case 8:
            case 9:
            case 10:
                model->baseMesh.indices[i] = 8;
                break;
            case 11:
                model->baseMesh.indices[i] = 11;
                break;
            case 12:
            case 13:
            case 14:
                model->baseMesh.indices[i] = 12;
                break;
            case 15:
            case 16:
            case 17:
                model->baseMesh.indices[i] = 15;
                break;
            case 18:
            case 19:
                model->baseMesh.indices[i] = 18;
                break;
            default:
                break;
                
        }
    }
#endif
        
    return model;
}

#define GAME_H
#endif
