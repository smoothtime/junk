#if !defined(GENERALALLOCATOR_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

enum memory_block_flags
{
    MEM_FLAG_USED = 0x01,
};

struct memory_block
{
    memory_block *next;
    memory_block *prev;
    uint64 size;
    uint8 flags;
};

memory_block *
addBlock(memory_block *prev, uint64 size, void *memory)
{
    memory_block *block = (memory_block *) memory;
    block->size = size - sizeof(memory_block);
    block->flags = 0;
    block->prev = prev;
    block->next = prev->next;
    block->next->prev = block;
    block->prev->next = block;

    return block;
}

bool32
combineFreeBlocks(memory_block *sentinel, memory_block *first)
{   
    bool32 combined = false;
    if(first != sentinel && first->next != sentinel)
    {
        if(!(first->flags & MEM_FLAG_USED) && !(first->next->flags & MEM_FLAG_USED))
        {
            if((uint8 *)first + sizeof(memory_block) +first->size == (uint8 *)first->next)
            {
                first->size += sizeof(memory_block) + first->next->size;
                first->next->prev = first;
                first->next = first->next->next;
                combined = true;
            }
        }
    }

    return combined;
}

struct GeneralAllocator
{
    memory_block memorySentinel;
    void *alloc(uint64 size, uint8 alignment);
    void dealloc(void *mem);
};

// NOTE(james): size includes whatever will be used by metadata
GeneralAllocator *
initGeneralAllocator(MemoryArena *arena, uint64 size)
{
    GeneralAllocator *result = PushStruct(arena, GeneralAllocator);
    result->memorySentinel.next = &result->memorySentinel;
    result->memorySentinel.prev = &result->memorySentinel;
    result->memorySentinel.size = 0;
    result->memorySentinel.flags = 0;
    addBlock(&result->memorySentinel, size, (void *)PushSize(arena, size));

    return result;
}

memory_block *
findFirstBlockForSize(memory_block *sentinel, uint64 requestedSize)
{
    memory_block *result = 0;
    for(memory_block *search = sentinel->next;
        search != sentinel;
        search = search->next)
    {
        if(!(search->flags & MEM_FLAG_USED))
        {
            if(search->size >= requestedSize)
            {
                result = search;
                break;
            }
        }
    }
    
    return result;
}

#define GENERALALLOCATOR_H
#endif
