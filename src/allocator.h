#if !defined(ALLOCATOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

class Allocator
{
public:
    virtual ~Allocator() {}
    virtual void *allocate(uint64 size) = 0;
    virtual void deallocate(void *ptr) = 0;

    template <class T> void
    allocateArray(uint64 arraySize, T*& result)
    {
        result = static_cast<T*>(allocate(sizeof(T) * arraySize));
    }
};

#define ALLOCATOR_H
#endif
