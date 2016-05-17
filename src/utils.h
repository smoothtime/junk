#if !defined(UTILS_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
struct JString
{
    uint32 size;
    uint8 *c;
};

internal JString *
readString(MemoryArena *memArena, uint8 *start)
{
    int32 size = 0;
    while(start && *(start + size) != '\0')
    {
        ++size;
    }
    ++size; // get the terminating
    JString *ret = PushStruct(memArena, JString);
    ret->size = size;
    ret->c = (uint8  *)PushSize(memArena, size * sizeof(uint8));
    memcpy(ret->c, start, size);
    return ret;
}

#define UTILS_H
#endif
