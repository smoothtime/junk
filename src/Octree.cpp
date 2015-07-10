/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Octree.h"

bool
Octree::isLeafNode()
{
    return children == 0;
}

void
Octree::insert(uint32 ent_index)
{
    insert(ent_index, 0);
}

void
Octree::insert(uint32 ent_index, int32 depth)
{
    Entity entity = (*entities)[ent_index];
    if(isLeafNode())
    {
        if(entityCount == 4 && depth < OCTREE_MAX_DEPTH)
        {
            // divide this box into another 8
            allocator->allocateArray(8, this->children);
            for(int i = 0; i < 8; i
                ++)
            {
                bool32 signX = (i & 1);
                bool32 signY = (i & 2);
                bool32 signZ = (i & 4);
                Vec3 minBound(signX ? origin.x : aabb._min.x,
                              signY ? origin.x : aabb._min.y,
                              signZ ? origin.z : aabb._min.z);
                Vec3 maxBound(signX ? aabb._max.x : origin.x,
                              signY ? aabb._max.y : origin.y,
                              signZ ? aabb._max.z : origin.z);
                Vec3 childOrg = minBound + ((maxBound - minBound) * 0.5);
                new (&children[i]) Octree(allocator, childOrg, AABBox(minBound, maxBound), entities);
            }
            
            // reinsert each entity into corresponding children
            for(int32 i = 0; i < 4; ++i)
            {
                insertToCollidedChildren(entityIndexes[i], depth);
            }
            
            // insert incoming entity into corresponding child
            insertToCollidedChildren(ent_index, depth);
            
        }
        else
        {
            // TODO(james): this will blow up once we reach max depth
            // need a dynamically sized data structure or memory arena of ids
            entityIndexes.push_back(ent_index);
        }
    }
    else
    {
        insertToCollidedChildren(ent_index, depth);
    }
    entityCount++;
}

uint8
Octree::whichOctant(const Vec3 &point)
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
    uint8 childOctantMin = whichOctant(bounds._min);
    uint8 childOctantMax = whichOctant(bounds._max);
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
            if(doBoundsCollide(children[i].aabb, bounds))
            {
                childOctants = childOctants | (1 << i);
            }
        }
            
    }

    return childOctants;
}

inline void
Octree::insertToCollidedChildren(uint32 ent_index, int32 depth)
{
    uint8 toInsert = whichChildren((*entities)[ent_index].aabb);
    for(int32 i = 0; i < 8; ++i)
    {
        if((toInsert >> i) & 1)
        {
            children[i].insert(ent_index, depth + 1);
        }
    }
}
