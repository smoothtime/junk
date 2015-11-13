#if !defined(LINEARALLOCATOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define LINEARALLOCATOR_H
class LinearAllocator : public BaseAllocator
{
public:
    LinearAllocator(uint32 identifier, size_t allocationSize, Allocator *backingAllocator);

    void *
    allocate(uint64 size, uint8 alignment = 4) override;

    void
    deallocate(void *ptr) override;

private:
    void *tail;
}
#endif
