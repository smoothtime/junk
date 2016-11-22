#if !defined(ENTITY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

struct Entity
{
    bool32 isStatic;
    uint32 entityID;
    glm::vec3 position;
    glm::mat4 rotMtx;
    glm::mat4 transMtx;
    Model *model;
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
    glm::vec3 origin = (entity->model->aabb.minBound + entity->model->aabb.maxBound);
    origin = origin * 0.5f;
    // and size
    glm::vec3 size = entity->model->aabb.maxBound - entity->model->aabb.minBound;
    
    hashValue = (uint32) (ceil(origin.x * 10 + origin.y * 5 + origin.z * 3
                                  - size.x * 10 - size.y * 5 - size.z * 3)
                          + entity->entityID);
    hashValue = hashValue & (1024 - 1);
    return hashValue;
}

struct IDNode
{
    uint32 id;
    IDNode *next;
};

struct IDSystem
{
    uint32 currentId;
    IDNode *returnedList;
    IDNode *freedFreeList;
    std::mutex mtx;
    
    uint32 getNextID(MemoryArena *mem);
    void returnID(MemoryArena *mem, uint32 id);
};

void initializeIDSystem();


#define ENTITY_H
#endif
