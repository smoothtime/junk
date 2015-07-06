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
	AABBox() {}
    AABBox(real32 x1, real32 y1, real32 z1, real32 x2, real32 y2, real32 z2) : _min(x1, y1, z1), _max(x2, y2, z2) {}
    AABBox(const Vec3 &min, const Vec3 &max) : _min(min), _max(max) {}

	static void *operator new(size_t size)
    {
        // Mainly want to be constructing vectors
        // with placement new with Allocator provided
        // memory addresses, but if we need to allocate
        // a vector on the fly, just make one on the global allocator
        return GlobalAllocator()->allocate(size);
        
    }

    static void operator delete(void *block)
    {
        GlobalAllocator()->deallocate(block);
    }
    
    Vec3 _min;
    Vec3 _max;
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

	static void *operator new(size_t size)
    {
        // Mainly want to be constructing vectors
        // with placement new with Allocator provided
        // memory addresses, but if we need to allocate
        // a vector on the fly, just make one on the global allocator
        return GlobalAllocator()->allocate(size);
        
    }

	static void *operator new(size_t size, void *ptr)
	{
		return ptr;
	}

    static void operator delete(void *block)
    {
        GlobalAllocator()->deallocate(block);
    }
    
    uint32 id;
    AABBox aabb;
    
};

bool32 doBoundsCollide(AABBox box1, AABBox box2)
{
    return ( (box1._min.x < box2._max.x) && (box1._max.x > box2._min.x)  &&
             (box1._min.y < box2._max.y) && (box1._max.y > box2._min.y)  &&
             (box1._min.z < box2._max.z) && (box1._max.z > box2._min.z) );
 
}

#define ENTITY_H
#endif
