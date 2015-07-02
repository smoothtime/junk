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
    // Define size of (sub)space
    Vec3 origin;
    AABBox aabb;

    Octree *children;
    uint32 entityIds[4];
    uint32 entityCount;

    int32 whichChild(const Vec3 &point);
    bool isLeafNode();
    void insert(Entity &entity, int32 depth);
public:
    void insert(Entity &entity);

};

#define OCTREE_H
#endif
