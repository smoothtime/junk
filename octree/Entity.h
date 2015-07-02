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
    AABBox(real32 x1, real32 y1, real32 z1, real32 x2, real32 y2, real32 z2) : min(x1, y1, z1), max(x2, y2, z2) {}
    AABBox(const Vec3 &_min, const Vec3 &_max) : min(_min), max(_max) {}
    
    Vec3 min;
    Vec3 max;
};

struct Entity
{
    Entity(uint32 _id, real32 x1, real32 y1, real32 z1, real32 x2, real32 y2, real32 z2)
            : id(_id), aabb(x1, y1, z1, x2, y2, z2)
    {
    }

    Entity(uint32 _id, const Vec3 &min, const Vec3 &max)
            : id(_id), aabb(min, max)
    {
    }

    Entity(uint32 _id, const AABBox &box)
            : id(_id), aabb(box)
    {
    }
    
    uint32 id;
    AABBox aabb;
    
};

bool32 doBoundsCollide(AABBox box1, AABBox box2)
{
    return ( (box1.min.x < box2.max.x) && (box1.max.x > box2.min.x)  &&
             (box1.min.y < box2.max.y) && (box1.max.y > box2.min.y)  &&
             (box1.min.z < box2.max.z) && (box1.max.z > box2.min.z) );
 
}

#define ENTITY_H
#endif
