#if !defined(CALLOCALLOCATOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

class  CallocAllocator : public Allocator
{
public:
    void *
    allocate(uint64 size, uint8 alignment)
    {
        assert(size);
        return calloc(sizeof(size_t), static_cast<size_t>(size));
    }

    void
    deallocate(void *ptr)
    {
        free(ptr);
    }
};
#define CALLOCALLOCATOR_H
#endif
