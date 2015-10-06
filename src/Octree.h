#if !defined(OCTREE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#include <vector>
#define OCTREE_MAX_DEPTH 10
struct Octree;

struct Octree
{
    Octree() {}
    Octree(Allocator *treeAllocator, Vec3 org, AABBox bounds, junk::JVector<Entity> *ents)
    {
        allocator = treeAllocator;
        origin = org;
        aabb = bounds;
        children = 0;
        entityCount = 0;
        entities = ents;
        new (&entityIndexes) junk::JVector<uint32>(4, treeAllocator);
    }
    
    Allocator *allocator;
    // Define size of (sub)space
    Vec3 origin;
    AABBox aabb;

    Octree *children;
    junk::JVector<Entity> *entities;
    junk::JVector<uint32> entityIndexes;
    //uint32 entityIndexes[4];
    uint32 entityCount;

    bool isLeafNode();
    void insert(uint32 ent_index);
    void insert(uint32 ent_index, int32 depth);
    void checkCollisions(uint32 ent_index, junk::JSet<uint32> &collisions);

private:
    uint8 whichOctant(const Vec3 &point);
    uint8 whichChildren(AABBox bounds);
    void insertToCollidedChildren(uint32 ent_index, int32 depth);

};

#define OCTREE_H
#endif
