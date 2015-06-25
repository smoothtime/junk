#if !defined(OCTREE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#include <vector>
#define OCTREE_MAX_DEPTH 10;
struct Octree;

struct Octree
{
    // Define size of (sub)space
    Vec3 origin;
    Vec3 half_dimensions;
    AABBox aabb;

    Octree *children;
    uint32 entityIds[4];
    uint32 entityCount;

    int32 whichChild(Vec3 *point);
    bool isLeafNode();

};

bool isLeafNode(Octree *oct)
{
    return oct->children == 0;
}

void insert(uint32 id, Vec3 *position, Octree *oct)
{
    // if a leaf node
    if(isLeafNode(oct))
    {
        // if all children are assigned, need to split and reinsert 
        if(oct->entityCount == 4)
        {
            oct->children = PushArray(&global_arena, 8, Octree);
            for(int i = 0; i < 8; i++)
            {
                // TODO(james): actual origin based on current half dim and origin
                Vec3 *new_origin = new Vec3(0.0f, 0.0f, 0.0f);
                oct->children[i].origin = *new_origin;
            }
            for(int i = 0; i < oct->entityCount; i++)
            {
                //Entity *ent = getDaEntity(i, entities);
                //insert(ent->id, ent->
            }
            int32 child = whichChild(position, oct);
            insert(id, position, &oct->children[child]);
        }
        else
        {
            oct->entityIds[oct->entityCount++] = id;
        }
    }
    else
    {
        int32 child = whichChild(position, oct);
        insert(id, position, &oct->children[child]);
    }
}

void insert(Entity *entity, Octree *oct)
{
    insert(entity->id, &entity->aabb.min, oct);
    insert(entity->id, &entity->aabb.max, oct);
}

#define OCTREE_H
#endif
