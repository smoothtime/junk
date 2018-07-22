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

internalfun JString *
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

bool
compareJStringToMemoryContent(JString jString, uint8 *sequence)
{
    for(uint32 i = 0; i < jString.size; i++) {
        if(jString.c[i] != sequence[i])
            return false;
    }

    return true;
}


bool
compareJStringToCString(JString jString, const char *cString)
{
    for(uint32 i = 0; i < jString.size; i++) {
        if(cString[i] == '\0' || (jString.c[i] != cString[i]))
            return false;
    }

    return true;
}

bool cStringEndsIn(const char *fileName, const char *fileExtension)
{
    int i = 0, j = 0;
    bool endsInExtension = true;
    while (fileName[i] != '\0')
    {
        i++;
    }
    while (fileExtension[j] != '\0')
    {
        j++;
    }

    while(j >= 0 && i >= 0) {
        if (fileName[i] != fileExtension[j]) {
            endsInExtension = false;
            break;
        }
            
        j--;
        i--;
    }
    
    return endsInExtension;
}


#define UTILS_H
#endif
