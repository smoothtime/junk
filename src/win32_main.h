#if !defined(WIN32_MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#undef _HAS_EXCEPTIONS
#define _HAS_EXCEPTIONS 0

#define ASSERT assert

#define WIN32_FILE_NAME_SIZE MAX_PATH

struct Win32GameDLL
{
    HMODULE gameCodeDLL;
    FILETIME dllLastWriteTime;

    // NOTE(james): Either of the callbacks can be 0!  You must
    // check before calling.
    GameUpdate *gameUpdate;

    bool32 isValid;
};

inline FILETIME
win32GetLastWriteTime(char *file)
{
    FILETIME lastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA data;
    if(GetFileAttributesEx(file, GetFileExInfoStandard, &data))
    {
        lastWriteTime = data.ftLastWriteTime;
    }

    return lastWriteTime;
}

static Win32GameDLL
win32LoadGameCode(char *srcDLL, char *tmpDLL, char *lock)
{
    Win32GameDLL result = {};
    WIN32_FILE_ATTRIBUTE_DATA ignored;
    if(!GetFileAttributesEx(lock, GetFileExInfoStandard, &ignored))
    {
        result.dllLastWriteTime = win32GetLastWriteTime(srcDLL);

        CopyFile(srcDLL, tmpDLL, FALSE);

        result.gameCodeDLL = LoadLibraryA(tmpDLL);
        if(result.gameCodeDLL)
        {
            result.gameUpdate = (GameUpdate *) GetProcAddress(result.gameCodeDLL, "gameUpdate");

            result.isValid = result.gameUpdate && 1;
        }
    }

    if(!result.isValid)
    {
        result.gameUpdate = 0;
    }

    return result;
}

static void
win32UnloadGameCode(Win32GameDLL *gameCode)
{
    if(gameCode->gameCodeDLL)
    {
        FreeLibrary(gameCode->gameCodeDLL);
        gameCode->gameCodeDLL = 0;
    }

    gameCode->isValid = false;
    gameCode->gameUpdate = 0;
}
#define WIN32_MAIN_H
#endif
