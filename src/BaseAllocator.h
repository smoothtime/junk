#if !defined(BASEALLOCATOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define BASEALLOCATOR_H
class BaseAllocator : public Allocator
{
public:
    BaseAllocator(uint32 identifier, Allocator *backingAllocator) : Allocator(identifier)
    {
        id = identifier;
        bAllocator = backingAllocator;
    }

protected:
    // allocator used for additional memory requests
    Allocator *bAllocator;
    uint32 id;
    size_t mStart;
    size_t mSize;
    size_t mUsedMemory;
    size_t mNumAllocs;
}
#endif
