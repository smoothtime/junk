/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "Octree.h"

int32 Octree::whichChild(Vec3 point)
{
    // children go form back bottom left to front top right
    int octant = 0;
    if(point->x > this->origin.x)
    {
        octant |= 1;
    }
    if(point->y > this->origin.y)
    {
        octant |= 2;
    }
    if(point->z > this->origin.z)
    {
        octant |= 4;
    }
}

bool Octree::isLeafNode()
{
    return children == 0;
}

void Octree::insert(Entity *entity)
{
    insert(entity, 0);
}

void Octree::insert(Entity *entity, int32 depth)
{
    if(isLeafNode())
    {
        if(entityCount == 4 && depth < OCTREE_MAX_DEPTH)
        {
            // try to split
            children = PushArray(&global_arena, 8, Octree);
            for(int i = 0; i < 8; i++)
            {
                real32 signX = (i & 1) ? 1.0f : -1.0f;
                real32 signY = (i & 2) ? 1.0f : -1.0f;
                real32 signZ = (i & 4) ? 1.0f : -1.0f;
                children[i].origin = Vec3(origin.x * signX,
                                          origin.y * signY,
                                          origin.z * signZ);
                children[i].half_dimensions = Vec3ScalarMult(0.5f, half_dimensions);
            }
        }
        else
        {
            // TODO(james): this will blow up once we reach max depth
            // need a dynamically sized data structure or memory arena of ids
            entityIds[entityCount++] = entityCount->id;
        }
    }
    else
    {
        int32 childOctantMin = whichChild(entity->min);
        int32 childOctantMax = whichChild(entity->max);
        if(childOctantMin == childOctantMax)
        {
            children[childOctantMax]->insert(entity, depth++);
        }
        else
        {
            // crosses a bound, need to check against all children
            for(int i = 0; i < 8; i++)
            {
                if(doBoundsCollide(children[i]->aabb, entity->aabb))
                {
                    children[i]->insert(entity, depth++);
                }
            }
            
        }
    }
}
