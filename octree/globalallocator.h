#if !defined(GLOBALALLOCATOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

MallocAllocator global_allocator;
Allocator *
GlobalAllocator()
{
    return &global_allocator;
}

#define GLOBALALLOCATOR_H
#endif
