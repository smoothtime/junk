#if !defined(LINUX_MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells$
   $Notice: (C) Copyright 2016 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

struct OSXDyLib
{
    GameUpdate *gameUpdate;
    bool32 isValid;
    int32 lastWritten;
};

OSXDyLib
osxLoadGameCode(const char *srcDyLib)
{
    OSXDyLib result = {};
    int32 flagP = RTLD_LAZY;
    void *libHandle = dlopen(srcDyLib, RTLD_NOW);
    if(!libHandle)
    {
        printf("Couldn't load %s: %s", srcDyLib, dlerror());
    }
    result.gameUpdate = (GameUpdate *)dlsym(libHandle, "gameUpdate");
    if(result.gameUpdate)
    {
        result.isValid = true;
        struct stat attr;
        stat(srcDyLib, &attr);
        result.lastWritten = attr.st_mtime;
    }
    return result;
}

bool32
hasDyLibUpdatedSinceLastRead(OSXDyLib lib, const char *path)
{
    struct stat attr;
    if(stat(path, &attr) == 0)
    {
        return false;
    }
    else
    {
        return attr.st_mtime > lib.lastWritten;
    }
}

#define LINUX_MAIN_H
#endif
