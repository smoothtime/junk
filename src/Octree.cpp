/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Octree.h"

Octree *
InitOctreeNode(AABBox bounds, MemoryArena *memArena)
{
    Octree *result = PushStruct(memArena, Octree);
    result->aabb = bounds;
    result->origin = (bounds.minBound + bounds.maxBound) * 0.5f;
    result->entityCount = 0;
    return result;
}

Octree *
ConstructOctree(AABBox bounds, uint32 depth, MemoryArena *memArena)
{
    Octree *node = InitOctreeNode(bounds, memArena);
    if(depth)
    {
        for(uint32 child = 0;
            child < 8;
            ++child)
        {
            bool32 signX = (child & 1);
            bool32 signY = (child & 2);
            bool32 signZ = (child & 4);
            glm::vec3 minBound = glm::vec3(signX ? node->origin.x : node->aabb.minBound.x,
                                 signY ? node->origin.x : node->aabb.minBound.y,
                                 signZ ? node->origin.z : node->aabb.minBound.z);
            glm::vec3 maxBound = glm::vec3(signX ? node->aabb.maxBound.x : node->origin.x,
                                 signY ? node->aabb.maxBound.y : node->origin.y,
                                 signZ ? node->aabb.maxBound.z : node->origin.z);
            AABBox childBounds = { minBound, maxBound };
            
            node->children[child] = ConstructOctree(childBounds, depth - 1, memArena);
        }
    }
    else
    {
        node->entityReferenceBlock = PushStruct(memArena, entity_block);
    }
    return node;
}

bool
Octree::isLeafNode()
{
    return children[0] == 0;
}

void
Octree::insert(Entity *entity, MemoryArena *memArena)
{
    if(isLeafNode())
    {
        uint32 hashIndex = entityHashFunction(entity);
        entity_reference *existingReference = entityReferenceBlock->entityReferences + hashIndex;
        if(existingReference->index == 0)
        {
            existingReference->index =  entity->entityIndex;
            entityReferenceBlock->entityCount++;
        }
        else
        {
            // External chaining
            while(existingReference->next)
            {
                existingReference = existingReference->next;
            }
            existingReference->next = PushStruct(memArena, entity_reference);
            existingReference->next->index = entity->entityIndex;
            existingReference->next->next = 0;
        }
    }
    else
    {
        insertToCollidedChildren(entity, memArena);
    }
    entityCount++;
}

inline bool32
alreadyCollided(uint32 *collisionIndices, uint32 numChecks, uint32 indexToCheck)
{
    for(uint32 i = 0; i < numChecks; ++i)
    {
        if(collisionIndices[i] == indexToCheck)
        {
            return true;
        }
    }
    return false;
}

void
Octree::checkCollisions(Entity *entityArray, Entity* entityToCheck, uint32 *collisionIndices, uint32 numChecks)
{
    uint32 collisionsSoFar = 0;
    checkCollisions(entityArray, entityToCheck, collisionIndices, numChecks, &collisionsSoFar);
}
    
void
Octree::checkCollisions(Entity *entityArray, Entity* entity, uint32 *collisionIndices, uint32 numChecks, uint32 *collisionsSoFar)
{
    if(isLeafNode())
    {
        for(uint32 i = 0; i < ArrayCount(entityReferenceBlock->entityReferences); ++i)
        {
            entity_reference *toCheck = entityReferenceBlock->entityReferences + i;
            do {
                if(toCheck->index)
                {

                    if(doBoundsCollide(entity->aabb, entityArray[toCheck->index].aabb)
                       && *collisionsSoFar < numChecks
                       && !alreadyCollided(collisionIndices, numChecks, toCheck->index)) //don't want to collide with same entity stretching across multiple children
                    { 
                        *(collisionIndices + *collisionsSoFar) = toCheck->index;
                        (*collisionsSoFar)++;
                    }
                }
                toCheck = toCheck->next;
            } while(toCheck);
        }
    }
    else
    {
        uint8 collidedChildren = whichChildren(entity->aabb);
        for(int32 i = 0; i < 8; ++i)
        {
            if((collidedChildren >> i) & 1)
            {
                children[i]->checkCollisions(entityArray, entity, collisionIndices, numChecks, collisionsSoFar);
            }
        }
    }
}

uint8
Octree::whichOctant(glm::vec3 point)
{
    // children go form back bottom left to front top right
    uint8 octant = 0;
    if(point.x > this->origin.x)
    {
        octant |= 1;
    }
    if(point.y > this->origin.y)
    {
        octant |= 2;
    }
    if(point.z > this->origin.z)
    {
        octant |= 4;
    }

    return octant;
}

uint8
Octree::whichChildren(AABBox bounds)
{
    uint8 childOctants;
    uint8 childOctantMin = whichOctant(bounds.minBound);
    uint8 childOctantMax = whichOctant(bounds.maxBound);
    if(childOctantMin == childOctantMax)
    {
        childOctants = 1 << childOctantMin;
        return childOctants;
    }
    else
    {
        // crosses a bound, need to check against all children
        for(int32 i = 0; i < 8; i++)
        {
            if(doBoundsCollide(children[i]->aabb, bounds))
            {
                childOctants = childOctants | (1 << i);
            }
        }
            
    }

    return childOctants;
}

inline void
Octree::insertToCollidedChildren(Entity *entity, MemoryArena *memArena)
{
    uint8 toInsert = whichChildren(entity->aabb);
    for(int32 i = 0; i < 8; ++i)
    {
        if((toInsert >> i) & 1)
        {
            children[i]->insert(entity, memArena);
        }
    }
}
