#if !defined(PLATFORM_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
//
// NOTE(james): Compilers
//
    
#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif
    
#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#else
// TODO(james): add gcc or whatever
#undef COMPILER_LLVM
#define COMPILER_LLVM 1
#endif
#endif

#include <assert.h>
#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdint.h>
#include <mutex>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;
typedef uintptr_t uptr;
#define PI32 3.14159265

#define internalfun static

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#if COMPILER_MSVC
#include <GLM\glm.hpp>
#define sproot sprintf_s
#else
#include <GLM/glm.hpp>
#define sproot sprintf
#endif

#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include "MemoryArena.h"
#include "utils.h"

typedef struct thread_context
{
    int placeholder;
} thread_context;

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct read_file
{
    uint64 size;
    void *memory;
} read_file;

#define FREE_FILE(functionName) void functionName(thread_context *thread, void* memory)
typedef FREE_FILE(platformServiceFreeFile);

#define READ_ENTIRE_FILE(functionName) read_file functionName(thread_context *thread, const char *filePath)
typedef READ_ENTIRE_FILE(platformServiceReadEntireFile);

#define PLATFORM_LOG(functionName) void functionName(const char *msg)
typedef PLATFORM_LOG(platformLog);

// global logger to be set by platform;
platformLog *gLog;

typedef struct GameMemory
{
    bool32 isInitialized;
    uint64 permanentStorageSize;
    uint64 transientStorageSize;
    void * permStorage;
    void * transStorage;

    platformServiceReadEntireFile *platformServiceReadFile;
    platformLog *log;
} GameMemory;

typedef struct GameInput
{
    bool32 wantsToTerminate;
    bool32 w, a, s, d, space, leftClick, newLeftClick, rightClick, newRightClick;
    real32 mouseX, mouseY;
    real32 mouseDeltaX, mouseDeltaY;
    uint32 resX, resY;
} GameInput;


#define GAME_UPDATE(functionName) void functionName(thread_context *thread, GameMemory *memory, GameInput *input, real64 deltaTime, bool32 reloadExtensions)
typedef GAME_UPDATE(GameUpdate);

struct RenderReferenceIndex {
    uint32 shaderIndex;
    uint32 textureIndex;
    uint32 VAOIndex;
    uint32 VBOIndex;
    uint32 EBOIndex;
    uint32 numIndices;
};

#define PLATFORM_H
#endif
