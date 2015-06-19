#if !defined(ONETYPEMEMORYARENA_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

struct OneTypeMemoryArena
{
    uint8 *mem_block;
    size_t element_size;
    size_t mem_size;
    size_t mem_used;
};

OneTypeMemoryArena
initializeOneTypeArena(size_t thing_size, uint64 size)
{
    OneTypeMemoryArena arena;
    arena.element_size = thing_size;
    arena.mem_size = thing_size * size;
    arena.mem_used = 0;
    arena.mem_block = (uint8 *) malloc(arena.mem_size);
    
    return arena;
}

inline void *
PushSize_(OneTypeMemoryArena *arena, size_t desired_size)
{
    Assert(arena->mem_used + desired_size <= arena->mem_size);
    void *result = arena->mem_block + arena->mem_used;
    arena->mem_used += desired_size;
    return result;
}





#define ONETYPEMEMORYARENA_H
#endif
