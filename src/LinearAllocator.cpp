/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "LinearAllocator.h"

LinearAllocator::LinearAllocator(uint32 identifier, size_t allocationSize, Allocator *backingAllocator)
        : BaseAllocator(identifier, backingAllocator)
{
    ASSERT(size > 0);
    mSize = allocationSize;
    mStart = backingAllocator->allocate(mSize);
    mUsedMemory = 0;
    mNumAllocs = 0;
    tail = mStart;
}

LinearAllocator::~LinearAllocator()
{
    tail = null;
}

void *
LinearAllocator::allocate(uint64 size, uint8 alignment = 4)
{
    ASSERT(size > 0);
    uint8 adjustment = memory_math::alignForwardAdjustment(tail, alignment);

    if(tail + adjustment + size > mSize)
    {
        return null;
    }

    void *alignedAddress = (void *) tail + adjustment;

    tail = (void *)(alignedAddress + size);

    mUsedMemory += adjustment + size;
    mNumAllocs++;

    return alignedAddress;
}

void
LinearAllocator::deallocate(void *ptr)
{
    mNumAllocs = 0;
    mUsedMemory = 0;
    mSize = 0;
    mStart = null;
    bAllocator->deallocate(ptr);
}
