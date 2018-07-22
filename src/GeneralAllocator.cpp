/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "GeneralAllocator.h"

internalfun size_t BLOCK_SPLIT_THRESHOLD = 256;
#define NEW(allocator, thing) (thing *) allocator->alloc(sizeof(thing))
void *
GeneralAllocator::alloc(uint64 size, uint8 alignment = 4)
{
    // TODO(james): actually care about alignment

    void *result = 0;
    memory_block *fittingBlock = findFirstBlockForSize(&this->memorySentinel, size);
    if(fittingBlock)
    {
        result = (uint8 *)(fittingBlock + 1); //incrememnt first, then cast
        fittingBlock->flags |= MEM_FLAG_USED;
        size_t remainingSpace = fittingBlock->size - size;
        if(remainingSpace >= BLOCK_SPLIT_THRESHOLD)
        {
            fittingBlock->size = size;
            addBlock(fittingBlock, remainingSpace, (uint8 *)result + size);
        }
        else
        {
            //TODO(james): record ununsed portion after block for use when mergin on free
        }
    }
    return result;
}

void
GeneralAllocator::dealloc(void *mem)
{
    memory_block *block = ((memory_block *) mem) - 1;
    block->flags &= ~MEM_FLAG_USED;
    if(!(block->prev->flags & MEM_FLAG_USED))
    {
        if(combineFreeBlocks(&this->memorySentinel, block->prev))
        {
            block = block->prev;
        }
        combineFreeBlocks(&this->memorySentinel, block);
    }
}
