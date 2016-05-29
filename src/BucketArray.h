#if !defined(BUCKETARRAY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define BUCKETARRAY_H

template <class T>
struct BucketArray
{
    uint32 elemSize;
    uint32 elemCount;
    uint32 bucketCount;
    uint8 *occupancy;
    T* elems;

    BucketArray initBucketArray(uint32 elementSize, uint32 numElems, uint32 numBuckets, MemoryArena *memArena);
    void addElem(T);
    T* getElem(uint32 bucketIndex, uint32 elemIndex);
};
#endif
