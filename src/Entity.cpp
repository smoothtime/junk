/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Entity.h"

AABBox
createBaseAABBox(Mesh *model)
{
    glm::vec3 minBounds = model->vertices[0].pos;
    glm::vec3 maxBounds = minBounds;

    for(uint32 v = 1;
        v < model->numVerts;
        ++v)
    {
        glm::vec3 comp = model->vertices[v].pos;
        if(comp.x < minBounds.x)
        {
            minBounds.x = comp.x;
        }
        if(comp.y < minBounds.y)
        {
            minBounds.y = comp.y;
        }
        if(comp.z < minBounds.z)
        {
            minBounds.z = comp.z;
        }
        if(comp.x > maxBounds.x)
        {
            maxBounds.x = comp.x;
        }
        if(comp.y > maxBounds.y)
        {
            maxBounds.y = comp.y;
        }
        if(comp.z > maxBounds.z)
        {
            maxBounds.z = comp.z;
        }
    }
    

    AABBox bounds = { minBounds, maxBounds };
    return bounds;
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

