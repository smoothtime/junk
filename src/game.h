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
    read_file loadedModel = psRF(thread, relPath);
    // Parse file
    uint16 *chunkId;
    uint32 *chunkLength;
    uint16 *count;
    uint16 *faceFlags;
    uint8 *readP = (uint8 *)loadedModel.memory;
    while(readP < (uint8 *)loadedModel.memory + loadedModel.size)
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
                model->numVerts = *((uint32 *)count);
                model->vertices = PushArray(&gameState->memArena, model->numVerts, Vertex);
                for(int32 i = 0; i < *count; ++i)
                {
                    model->vertices[i].pos = glm::vec3( *(real32 *)readP + 0,
                                                        *(real32 *)readP + 1 * sizeof(real32),
                                                        *(real32 *)readP + 2 * sizeof(real32)
                                                      );
                    readP += 3 * sizeof(real32);
                }
                break;
            }
            case CHUNK_FACE_LIST:
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                model->numIndices = *(uint32 *)count * 3;
                model->indices = PushArray(&gameState->memArena, model->numIndices, uint32);
                for(uint32 i = 0; i < model->numIndices; i+=3)
                {
                    model->indices[i + 0] = (uint32)*(uint16 *)readP;
                    model->indices[i + 1] = (uint32)*(uint16 *)readP + 1;
                    model->indices[i + 2] = (uint32)*(uint16 *)readP + 2;
                    // pass a not important face flag
                    faceFlags = (uint16 *) readP;
                    readP += 4 * sizeof(uint16);
                }
                break;
            case CHUNK_MAPPING_COORDINATES:
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                for(uint32 i = 0; i < model->numVerts; ++i)
                {
                    model->vertices[i].texCoords = glm::vec2( *(real32 *)readP + 0,
                                                              *(real32 *)readP + 1 * sizeof(real32)
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

    gameState->numModels++;
    return model;

    // Hacking to test
    /*Model model = {};
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
    
    // </hacking>

    gameState->numModels++;
    */
    
}

#define GAME_H
#endif
