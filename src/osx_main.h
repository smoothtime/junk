#if !defined(OSX_MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */


struct OSXDyLib
{
    GameUpdate *gameUpdate;
    bool32 isValid;
};

OSXDyLib
osxLoadGameCode(const char *srcDyLib)
{
    OSXDyLib result = {};
    int32 flagP = RTLD_LAZY;
    void *libHandle = dlopen(srcDyLib, RTLD_NOW);
    result.gameUpdate = (GameUpdate *)dlsym(libHandle, "gameUpdate");
    if(result.gameUpdate)
    {
        result.isValid = true;
    }
    return result;
}

bool32
CompareFileTime(const char *path1, const char *path2)
{
    struct stat attr1, attr2;
    if(stat(path1, &attr1) != 0 ||
       stat(path2, &attr2) != 0)
        return false;
    return attr1.st_mtime == attr2.st_mtime;
}

#define OSX_MAIN_H
#endif
