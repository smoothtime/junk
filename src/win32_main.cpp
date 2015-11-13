/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2014 by Molly Rocket, Inc. All Rights Reserved. $
   ======================================================================== */
#include <windows.h>
#include <stdint.h>
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

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <cassert>
#include "allocator.h"
#include "mallocallocator.h"
#include "globalallocator.h"
#include "jset.h"
#include "jvector.h"
#include "Vec3.h"
#include "Entity.h"
#include "Octree.cpp"
#include "radixsort.h"

static junk::JVector<Entity> entities(GlobalAllocator());

static LRESULT CALLBACK
Win32MainWindowCallback(HWND window,
                        UINT message,
                        WPARAM wParam,
                        LPARAM lParam)
{
    LRESULT result = 0;
    switch(message)
    {
        case WM_KEYUP:
            // idk man
            break;
        default:
        {
            result = DefWindowProcA(window, message, wParam, lParam);
        } break;
    }
    return result;
}

static void
Win32ProcessPendingMessages()
{
    MSG message;
    while(PeekMessage(&message, 0 , 0 , 0, PM_REMOVE))
    {
        switch(message.message)
        {
            case WM_KEYUP:
                // whatever I guess
                break;
            default:
            {
                TranslateMessage(&message);
                DispatchMessageA(&message);
            }
        }
    }
}

void
JVectorTest(Allocator *mallocator)
{
    junk::JVector<int32> *test1 = new junk::JVector<int32>(mallocator);
    test1->push_back(4);
    test1->push_back(2);
    test1->push_back(1);
    test1->push_back(0);
    test1->push_back(-1);
    int32 four = (*test1)[0];
    int32 first = *test1->begin();
    int32 last = *test1->end();
    size_t length = test1->size();
    test1->resize(length + 1);
    test1->resize(4);
    test1->resize(16, -7);
}

int CALLBACK
WinMain(HINSTANCE instance,
        HINSTANCE prevInstance,
        LPSTR commandLine,
        int showCode)
{
    WNDCLASSA WindowClass = {};

    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = instance;
    WindowClass.lpszClassName = "JunkWindowClass";

    if(RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(0,
                                      WindowClass.lpszClassName,
                                      "Junk",
                                      WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT,
                                      0,
                                      0,
                                      instance,
                                      0);

        if(Window)
        {
            int x = 1;
            MallocAllocator mallocator;
            while(x == 1)
            {
                Win32ProcessPendingMessages();              

                Vec3 vec1(-0.5f, -0.5f, -0.5f);
                Vec3 vec2( 0.5f,  0.5f,  0.5f);
                Vec3 origin = vec1 + ((vec2 - vec1) * 0.5f);

                AABBox box1(vec1, vec2);
                AABBox box2(0.01f, 0.01f, 0.01f, 0.05f, 0.05f, 0.05f); 
    
                uint32 entityIds[5] = {0};
                uint32 entityCount = 0;

                entities.push_back(Entity(1, box2));

                entities.push_back(Entity(2, AABBox(box2._min * 2.0f, box2._max * 2.0)));
                entities.push_back(Entity(3, AABBox(box2._min * 3.0f, box2._max * 3.0)));
                entities.push_back(Entity(4, AABBox(box2._min * 4.0f, box2._max * 4.0)));
                entities.push_back(Entity(5, AABBox(box2._min * 5.0f, box2._max * 5.0)));

                Octree tree(&mallocator, origin, AABBox(vec1, vec2), &entities);
                for(uint32 i = 0; i < entities.size(); ++i)
                {
                    tree.insert(i);
                }
            }
        }
    }
}
    
