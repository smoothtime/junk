/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#include "BucketArray.h"

template <class T>
BucketArray BucketArray<T>::initBucketArray(uint32 elementSize, uint32 numElems, uint32 numBuckets, MemoryArena *arena)
{
    elemSize = elementSize;
    elemCount = numElems;
    bucketCount = numBuckets;
    occupancy = PushArray(arena, elemCount * bucketCount, uint8);
}

template <class T>
T* BucketArray<T>::getElem(uint32 bucketIndex, uint32 elemIndex)
{
    return
}
