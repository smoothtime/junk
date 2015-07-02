#if !defined(RADIXSORT_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */


void
countSort(uint32 *toSort, uint32 length, uint32 mask)
{
    uint32 sorted[length];
    uint32 counts[2] = {0};

    for(int i = 0; i < length; ++i)
    {
        sorted[i] = 0;
        ++counts[(toSort[i] / mask) & 1];
    }

    counts[1] += counts[0];

    for(int i = length - 1; i >= 0; --i)
    {
        sorted[counts[(toSort[i] / mask) & 1] - 1] = toSort[i];
        --counts[(toSort[i] / mask) & 1];
    }

    for(int i = 0; i < length; ++i)
    {
        toSort[i] = sorted[i];
        printf("%d: %d\n", i, sorted[i]);
    }
    
}

#define RADIXSORT_H
#endif
