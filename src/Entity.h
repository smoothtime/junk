#if !defined(ENTITY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

struct AABBox
{    
    glm::vec3 minBound;
    glm::vec3 maxBound;
};

struct Entity
{
    bool32 isStatic;
    uint32 entityIndex;
    AABBox aabb;
    glm::vec3 position;
    glm::mat4 rotMtx;
    glm::mat4 transMtx;
    RenderReferenceIndex renderInfo;
};

bool32
doBoundsCollide(AABBox box1, AABBox box2)
{
    return ( (box1.minBound.x < box2.maxBound.x) && (box1.maxBound.x > box2.minBound.x)  &&
             (box1.minBound.y < box2.maxBound.y) && (box1.maxBound.y > box2.minBound.y)  &&
             (box1.minBound.z < box2.maxBound.z) && (box1.maxBound.z > box2.minBound.z) );
 
}

uint32
entityHashFunction(Entity *entity)
{
    // map entity properties down to space of 1024
    uint32 hashValue;

    // take into account origin of object
    glm::vec3 origin = (entity->aabb.minBound + entity->aabb.maxBound);
    origin = origin * 0.5f;
    // and size
    glm::vec3 size = entity->aabb.maxBound - entity->aabb.minBound;
    
    hashValue = (uint32) (ceil(origin.x * 10 + origin.y * 5 + origin.z * 3
                                  - size.x * 10 - origin.y * 5 + origin.z * 3)
                          + entity->entityIndex);
    hashValue = hashValue & (1024 - 1);
    return hashValue;
}

#define ENTITY_H
#endif
