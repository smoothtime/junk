/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Entity.h"

void
updateAABBox(AABBox *box, Mesh *mesh)
{
    for(uint32 v = 0;
        v < mesh->numVerts;
        ++v)
    {
        glm::vec3 *comp = &mesh->vertices[v].pos;
        if(comp->x < box->minBound.x)
        {
            box->minBound.x = comp->x;
        }
        if(comp->y < box->minBound.y)
        {
            box->minBound.y = comp->y;
        }
        if(comp->z < box->minBound.z)
        {
            box->minBound.z = comp->z;
        }
        if(comp->x > box->maxBound.x)
        {
            box->maxBound.x = comp->x;
        }
        if(comp->y > box->maxBound.y)
        {
            box->maxBound.y = comp->y;
        }
        if(comp->z > box->maxBound.z)
        {
            box->maxBound.z = comp->z;
        }
    }
}

AABBox
createBaseAABBox(Mesh *mesh)
{
    AABBox result = {};
    result.minBound = mesh->vertices[0].pos;
    result.maxBound = result.minBound;

    updateAABBox(&result, mesh);
    return result;
}

AABBox
transformAABB(glm::mat4 transform, AABBox *box)
{
    AABBox result;

    real32 deltaX = box->maxBound.x - box->minBound.x;
    real32 deltaY = box->maxBound.y - box->minBound.y;
    real32 deltaZ = box->maxBound.z - box->minBound.z;

    glm::vec3 originalVertices [8] = { box->minBound,
                                       box->minBound + glm::vec3(deltaX, 0.0f,   0.0f),
                                       box->minBound + glm::vec3(deltaX, deltaY, 0.0f),
                                       box->minBound + glm::vec3(deltaX, 0.0f,   deltaZ),
                                       box->minBound + glm::vec3(0.0f,   deltaY, 0.0f),
                                       box->minBound + glm::vec3(0.0f,   deltaY, deltaZ),
                                       box->minBound + glm::vec3(0.0f,   0.0f,   deltaZ),
                                       box->maxBound
    };

    glm::vec3 newMin = glm::vec3(transform * glm::vec4(originalVertices[0], 1.0f));
    glm::vec3 newMax = newMin;
    for(uint32 i = 1;
        i < 8;
        ++i)
    {
        glm::vec3 comp = glm::vec3(transform * glm::vec4(originalVertices[i], 1.0f));
        if(comp.x < newMin.x)
        {
            newMin.x = comp.x;
        }
        if(comp.y < newMin.y)
        {
            newMin.y = comp.y;
        }
        if(comp.z < newMin.z)
        {
            newMin.z = comp.z;
        }
        if(comp.x > newMax.x)
        {
            newMax.x = comp.x;
        }
        if(comp.y > newMax.y)
        {
            newMax.y = comp.y;
        }
        if(comp.z > newMax.z)
        {
            newMax.z = comp.z;
        }
    }
    
    result.minBound = newMin;
    result.maxBound = newMax;
    return result;
}

IDSystem *
initializeIDSystem(MemoryArena *mem)
{
    IDSystem *system = PushStruct(mem, IDSystem);
    system->currentId = 1;
    std::mutex *mtx = new(&system->mtx) std::mutex;
    
    return system;
}

uint32
IDSystem::getNextID(MemoryArena *mem)
{
    mtx.lock();
    uint32 ret;
    if(returnedList != NULL)
    {
        IDNode *justUsed = returnedList;
        ret = justUsed->id;
        
        returnedList = returnedList->next;
        justUsed->next = freedFreeList;
        freedFreeList = justUsed;

    }
    else
    {
        ret = currentId++;
    }
    mtx.unlock();
    return ret;
}

void
IDSystem::returnID(MemoryArena *mem, uint32 id)
{
    mtx.lock();
    IDNode *node;
    if(freedFreeList != NULL)
    {
        node = freedFreeList;
        freedFreeList = freedFreeList->next;
    }
    else
    {
        node = PushStruct(mem, IDNode);
    }
    node->id = id;
    node->next = returnedList;
    returnedList = node;
    mtx.unlock();
}
