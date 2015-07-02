/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Octree.h"

int32 Octree::whichChild(const Vec3 &point)
{
    // children go form back bottom left to front top right
    int octant = 0;
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

bool Octree::isLeafNode()
{
    return children == 0;
}

void Octree::insert(Entity &entity)
{
    insert(entity, 0);
}

void Octree::insert(Entity &entity, int32 depth)
{
    if(isLeafNode())
    {
        if(entityCount == 4 && depth < OCTREE_MAX_DEPTH)
        {
            // try to split
            GlobalAllocator()->allocateArray(8, this->children);
            for(int i = 0; i < 8; i++)
            {
                bool32 signX = (i & 1);
                bool32 signY = (i & 2);
                bool32 signZ = (i & 4);
                children[i].aabb.min = Vec3(signX ? origin.x : aabb.min.x,
                                       signY ? origin.x : aabb.min.y,
                                       signZ ? origin.z : aabb.min.z);
                children[i].aabb.max = Vec3(signX ? aabb.max.x : origin.x,
                                       signY ? aabb.max.y : origin.y,
                                       signZ ? aabb.max.z : origin.z);
                children[i].origin = ((children[i].aabb.max - children[i].aabb.min) * 0.5);
                children[i].aabb = AABBox(children[i].aabb.min, children[i].aabb.max);
            }

            // reinsert each entity into corresponding children
        }
        else
        {
            // TODO(james): this will blow up once we reach max depth
            // need a dynamically sized data structure or memory arena of ids
            entityIds[entityCount] = entity.id;
        }
    }
    else
    {
        int32 childOctantMin = whichChild(entity.aabb.min);
        int32 childOctantMax = whichChild(entity.aabb.max);
        if(childOctantMin == childOctantMax)
        {
            children[childOctantMax].insert(entity, depth++);
        }
        else
        {
            // crosses a bound, need to check against all children
            for(int i = 0; i < 8; i++)
            {
                if(doBoundsCollide(children[i].aabb, entity.aabb))
                {
                    children[i].insert(entity, depth++);
                }
            }
            
        }
    }
    entityCount++;
}
