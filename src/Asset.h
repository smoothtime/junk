#if !defined(ASSET_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define ASSET_H
#endif


Model *
loadAndParse3ds(thread_context *thread, GameState *gameState,
                platformServiceReadEntireFile *psRF, const char *relPath,
                Model *result, bool32 isCollisionMesh)
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

Model *
loadAndParseBlend(thread_context *thread, GameState *gameState,
                platformServiceReadEntireFile *psRF, const char *relPath,
                Model *result, bool32 isCollisionMesh)
{
    GeneralAllocator *alctr = gameState->assetAlctr;
    
    uint32 meshCount = 0;
    readMeshListEntry *first = NEW(alctr, readMeshListEntry);
    readMeshListEntry *current = first;

    Mesh *currentMesh = NEW(alctr, Mesh);
    current->entry = &currentMesh;
    
    read_file loadedFile = psRF(thread, relPath);
    uint8 *readP = (uint8 *)loadedFile.memory;

    // Header
    eastl::basic_string<char8_t> eheader = "BLENDER";
    assert(memcmp(eheader.data(), readP, 7) == 0);
    readP +=7;
    if (*readP != '-') {
        gLog("sorry I'm not dealing with 32 bit OSes today");
        assert(false);
    }
    readP++;
    if (*readP != 'v') {
        gLog("sorry I'm not dealing with Big Endian OSes today");
        assert(false);
    }
    readP++;
    // skipping version
    readP += 3;
    uint8 pointer_size = 8;

    // Align to 4 byte boundary
    readP += ((uint64)readP) % 4;

    // File block parsing
    uint8 *fbreadP;
    eastl::basic_string<char8_t> fb_identifier;
    eastl::basic_string<char8_t> log;
    uint32 blockSize;
    uint32 sdnaIdx;
    uint32 structureCount;
    while(readP < (uint8 *)loadedFile.memory + loadedFile.size)
    {
        // we're in a file block
        fb_identifier.assign((char8_t *)readP, 4);
        readP += 4;
        eastl::snprintf(log, "");
        gLog("\n file block: ");
        gLog(fb_identifier.c_str());
        blockSize = *(uint32 *)readP;
        fbreadP = readP;
        readP += 4;

        //skipping previous memory address
        readP += pointer_size;
        
        sdnaIdx = *(uint32 *)readP;
        readP += 4;
        structureCount = *(uint32 *)readP;
        readP += 4;
        readP += blockSize;
    }
    return 0;
}


Model *loadModel(thread_context *thread, GameState *gameState,
                 platformServiceReadEntireFile *psRF, Renderable *rendFiles)
{
    Model *result = gameState->models + gameState->numModels;
    if (cStringEndsIn(rendFiles->renderModelFileName, ".3ds")) {
        loadAndParse3ds(thread, gameState, psRF, rendFiles->renderModelFileName, result, false);
    } else if (cStringEndsIn(rendFiles->renderModelFileName, ".blend")) {
        loadAndParseBlend(thread, gameState, psRF, rendFiles->renderModelFileName, result, false);
    }
        
    for(uint32 meshIdx = 0; meshIdx < result->numRenderMesh; ++meshIdx)
    {
        Mesh *m = result->renderMeshes[meshIdx].mesh;
        uint32 sIdx = initShader(gameState->rendRefs, rendFiles->vShaderFileName, rendFiles->fShaderlFileName);
        uint32 tIdx = initTexture(gameState->rendRefs, rendFiles->textureFileName);
        uint32 vIdx = initVertexIndexBuffers(gameState->rendRefs, m);
        result->renderMeshes[meshIdx].rri = { sIdx, tIdx, vIdx, vIdx, vIdx, m->numIndices };
    }
    if (cStringEndsIn(rendFiles->renderModelFileName, ".3ds")) {
        loadAndParse3ds(thread, gameState, psRF, rendFiles->collisionModelFileName, result, true);
    } else if (cStringEndsIn(rendFiles->renderModelFileName, ".blend")) {
        loadAndParseBlend(thread, gameState, psRF, rendFiles->collisionModelFileName, result, true);
    }

    gameState->numModels++;

    return result;
}
