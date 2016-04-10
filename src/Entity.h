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
    Vec3 minBound;
    Vec3 maxBound;
};

struct Entity
{   
    uint32 entityIndex;
    AABBox aabb;
    //Vertex *verts;
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
    Vec3 origin = (entity->aabb.minBound + entity->aabb.maxBound) * 0.5;
    // and size
    Vec3 size = entity->aabb.maxBound - entity->aabb.minBound;
    
    hashValue = (uint32) (ceil(origin.x * 10 + origin.y * 5 + origin.z * 3
                                  - size.x * 10 - origin.y * 5 + origin.z * 3)
                          + entity->entityIndex);
    hashValue = hashValue & (1024 - 1);
    return hashValue;
}

#define ENTITY_H
#endif
