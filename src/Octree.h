#if !defined(OCTREE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#define OCTREE_MAX_DEPTH 8

struct entity_reference
{
    uint32 index;
    entity_reference *next;
};

struct entity_block
{
    uint32 entityCount;
    entity_reference entityReferences[1024];
};

struct Octree
{
    
    // Define size of (sub)space
    glm::vec3 origin;
    AABBox aabb;

    Octree *children[8];
    entity_block *entityReferenceBlock;
    uint32 entityCount;

    bool isLeafNode();
    void insert(Entity *entity, MemoryArena *memArena);
    uint8 whichOctant(glm::vec3 point);
    uint8 whichChildren(AABBox bounds);
    void insertToCollidedChildren(Entity *entity, MemoryArena *memArena);
    void checkCollisions(Entity *entityArray, Entity *entity, uint32 *collisionIndices, uint32 numChecks);
    void checkCollisions(Entity *entityArray, Entity *entity, uint32 *collisionIndices, uint32 numChecks, uint32 *collisionsSoFar);

};

#define OCTREE_H
#endif
