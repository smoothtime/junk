#if !defined(MALLOCALLOCATOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

class MallocAllocator : public Allocator
{
public:
    void *
    allocate(uint64 size)
    {
        assert(size);
        return malloc(static_cast<size_t>(size));
    }

    void
    deallocate(void *ptr)
    {
        free(ptr);
    }
};

#define MALLOCALLOCATOR_H
#endif
