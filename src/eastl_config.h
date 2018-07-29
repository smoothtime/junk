#if !defined(EASTL_CONFIG_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define EASTL_CONFIG_H
#endif
#define EASTL_EASTDC_VSNPRINTF 0
#include "EABase/eabase.h"
#include <stddef.h>
#include <new>
#include "EASTL/string.h"
using namespace eastl;

void* operator new[](size_t size, const char* /*name*/, int /*flags*/,
                    unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    return malloc(size);
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* /*name*/, 
                     int flags, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/)
{
    return malloc(size);
    //return malloc_aligned(size, alignment, alignmentOffset);
}

void* operator new(size_t size)
{
    return malloc(size);
}


void* operator new[](size_t size)
{
    return malloc(size);
}


///////////////////////////////////////////////////////////////////////////////
// Operator delete, which is shared between operator new implementations.
///////////////////////////////////////////////////////////////////////////////

void operator delete(void* p)
{
    if(p) // The standard specifies that 'delete NULL' is a valid operation.
        free(p);
}


void operator delete[](void* p)
{
    if(p)
        free(p);
}

int __cdecl Vsnprintf8(char8_t* p, size_t n, const char8_t* pFormat, va_list arguments)
{
        return vsnprintf(p, n, pFormat, arguments);
}

