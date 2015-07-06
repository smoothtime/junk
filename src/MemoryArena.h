#if !defined(MEMORYARENA_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

struct MemoryArena
{
    uint8 *mem_block;
    size_t mem_size;
    size_t mem_used;
};

MemoryArena
initializeArena(size_t desired_size)
{
    MemoryArena arena;
    arena.mem_size = desired_size;
    arena.mem_used = 0;
    arena.mem_block = (uint8 *) malloc(desired_size);
    
    return arena;
}

#define PushStruct(arena, type) (type *) PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type *)PushSize_(arena, (count)*sizeof(type))
#define PushDoublePointer(arena, count, type) (type **)PushSize_(arena, (count)*sizeof(type))
#define PushSize(arena, size) PushSize_(arena, size);
inline void *
PushSize_(MemoryArena *arena, size_t desired_size)
{
    Assert(arena->mem_used + desired_size <= arena->mem_size);
    void *result = arena->mem_block + arena->mem_used;
    arena->mem_used += desired_size;
    return result;
}



#define MEMORYARENA_H
#endif
