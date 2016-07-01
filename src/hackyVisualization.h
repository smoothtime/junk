#if !defined(HACKYVISUALIZATION_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define HACKY_VIS_IDX 1
void
initHackyVisModel(GameState *gameState, Camera * cam, glm::vec3 rayDir)
{
    GeneralAllocator *alctr = gameState->assetAlctr;
    Model *clickVisModel = gameState->models + HACKY_VIS_IDX;
    Mesh *clickVis;
    Mesh *baseMesh;
    Mesh *worldMesh;
    
    bool32 needsToBeAllocated = (clickVisModel->numRenderMesh == 0);
    if(needsToBeAllocated)
    {
        clickVisModel->numRenderMesh = 1;
        clickVisModel->numCollisionMesh = 1;
        clickVisModel->renderMeshes = (RenderMesh *) alctr->alloc(sizeof(RenderMesh));
        clickVisModel->collisionMeshes = (CollisionMeshPair *) alctr->alloc(sizeof(CollisionMeshPair));
        
        clickVis = NEW(alctr, Mesh);
        clickVisModel->renderMeshes[0].mesh = clickVis;
        
        clickVis->numVerts = 8;
        clickVis->numIndices = 36;
        clickVis->vertices = (Vertex *) alctr->alloc(sizeof(Vertex) * clickVis->numVerts);
        clickVis->indices = (uint32 *) alctr->alloc(sizeof(uint32) * clickVis->numIndices);
                
        baseMesh = NEW(alctr, Mesh);
        worldMesh = NEW(alctr, Mesh);
        clickVisModel->collisionMeshes[0].baseMesh = baseMesh;
        clickVisModel->collisionMeshes[0].worldMesh = worldMesh;

        baseMesh->numVerts = clickVis->numVerts;
        baseMesh->numIndices = clickVis->numIndices;
        worldMesh->numVerts = clickVis->numVerts;
        worldMesh->numIndices = clickVis->numIndices;
        
        baseMesh->vertices = (Vertex *) alctr->alloc(sizeof(Vertex) * clickVis->numVerts);
        worldMesh->vertices = (Vertex *) alctr->alloc(sizeof(Vertex) * clickVis->numVerts);

        baseMesh->indices = (uint32 *) alctr->alloc(sizeof(uint32) * clickVis->numIndices);
        worldMesh->indices = (uint32 *) alctr->alloc(sizeof(uint32) * clickVis->numIndices); 
    }


    clickVis = clickVisModel->renderMeshes[0].mesh;
    baseMesh = clickVisModel->collisionMeshes[0].baseMesh;
    worldMesh = clickVisModel->collisionMeshes[0].worldMesh;

    clickVis->vertices[0].pos = cam->position + (-0.05f * cam->right) +  (0.05f * cam->up)  + glm::vec3(rayDir);
    clickVis->vertices[1].pos = cam->position + (0.05f * cam->right) +  (0.05f * cam->up)   + glm::vec3(rayDir);
    clickVis->vertices[2].pos = cam->position + (-0.05f * cam->right) +  (-0.05f * cam->up) + glm::vec3(rayDir);
    clickVis->vertices[3].pos = cam->position + (0.05f * cam->right) +  (-0.05f * cam->up)  + glm::vec3(rayDir);
    clickVis->vertices[4].pos = clickVis->vertices[0].pos  + 5.0f * glm::vec3(rayDir);
    clickVis->vertices[5].pos = clickVis->vertices[1].pos  + 5.0f * glm::vec3(rayDir);
    clickVis->vertices[6].pos = clickVis->vertices[2].pos  + 5.0f * glm::vec3(rayDir);
    clickVis->vertices[7].pos = clickVis->vertices[3].pos  + 5.0f * glm::vec3(rayDir);

    for(uint8 i = 0;
        i < clickVis->numVerts;
        ++i)
    {
        baseMesh->vertices[i].pos = clickVis->vertices[i].pos;
    }

    clickVis->indices[0] = 0;
    clickVis->indices[1] = 1;
    clickVis->indices[2] = 2;
    clickVis->indices[3] = 2;
    clickVis->indices[4] = 1;
    clickVis->indices[5] = 3;
    clickVis->indices[6] = 3;
    clickVis->indices[7] = 1;
    clickVis->indices[8] = 5;
    clickVis->indices[9] = 5;
    clickVis->indices[10] = 7;
    clickVis->indices[11] = 3;
    clickVis->indices[12] = 3;
    clickVis->indices[13] = 7;
    clickVis->indices[14] = 2;
    clickVis->indices[15] = 2;
    clickVis->indices[16] = 7;
    clickVis->indices[17] = 6;
    clickVis->indices[18] = 6;
    clickVis->indices[19] = 7;
    clickVis->indices[20] = 5;
    clickVis->indices[21] = 5;
    clickVis->indices[22] = 4;
    clickVis->indices[23] = 6;
    clickVis->indices[24] = 6;
    clickVis->indices[25] = 4;
    clickVis->indices[26] = 0;
    clickVis->indices[27] = 0;
    clickVis->indices[28] = 2;
    clickVis->indices[29] = 6;
    clickVis->indices[30] = 0;
    clickVis->indices[31] = 4;
    clickVis->indices[32] = 1;
    clickVis->indices[33] = 1;
    clickVis->indices[34] = 4;
    clickVis->indices[35] = 5;
            
    if(needsToBeAllocated)
    {
        initVertexIndexBuffers(gameState->rendRefs, clickVis);
        gameState->dynamicEntities[5] = {};
    }
    else
    {
        RenderReferenceIndex toClear = { HACKY_VIS_IDX, HACKY_VIS_IDX, HACKY_VIS_IDX, HACKY_VIS_IDX, HACKY_VIS_IDX };
        overrideVertexBuffers(gameState->rendRefs, toClear, clickVis);
    }
    gameState->dynamicEntities[5].model = clickVisModel;
    gameState->dynamicEntities[5].model->aabb = createBaseAABBox(clickVis);
    AABBox what = gameState->dynamicEntities[5].model->aabb;
    assert(what.minBound.x < what.maxBound.x);
    assert(what.minBound.y < what.maxBound.y);
    assert(what.minBound.z < what.maxBound.z);
}

#define HACKYVISUALIZATION_H
#endif
