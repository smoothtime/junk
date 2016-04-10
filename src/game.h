#if !defined(GAME_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "platform.h"
#include "Vec3.h"
#include "matrix.h"
#include "Entity.h"
#include "Octree.cpp"
#include "glRender.h"

struct Frustum
{
    real32 n;
    real32 f;
    real32 l;
    real32 r;
    real32 t;
    real32 b;
};

struct Camera
{
    Vec3 position;
    Frustum frustum;
    mat4 viewMatrix;
};

inline mat4
createPerspectiveMatrix(Camera *camera)
{
    mat4 result = perspectiveMatrix(camera->frustum.n, camera->frustum.f,
                                    camera->frustum.r, camera->frustum.l,
                                    camera->frustum.t, camera->frustum.b);    
    return result;
}

struct GameState
{
    MemoryArena memArena;
    Entity staticEntities[10000];
    uint32 entityCount;
    Octree *staticEntityTree;
    Camera camera;
    Vec3 testDelta;
    RenderReferences *rendRefs;
};

#define GAME_H
#endif
