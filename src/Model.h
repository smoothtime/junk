#if !defined(MESH_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

struct Vertex
{
    glm::vec3 pos;
    glm::vec2 texCoords;    
};

struct AABBox
{    
    glm::vec3 minBound;
    glm::vec3 maxBound;
};

struct Mesh
{
    uint32 numVerts;
    Vertex *vertices;
    uint32 numIndices;
    uint32 *indices;
};

struct RenderMesh
{
    Mesh *mesh;
    RenderReferenceIndex rri;
};

struct CollisionMeshPair
{
    Mesh *baseMesh;
    Mesh *worldMesh;
};

struct Model
{
    AABBox aabb;
    uint32 numRenderMesh;
    RenderMesh *renderMeshes;
    uint32 numCollisionMesh;
    CollisionMeshPair *collisionMeshes;
};

struct Asset
{
    const char *renderModelFileName;
    const char *collisionModelFileName;
    const char *vShaderFileName;
    const char *fShaderlFileName;
    const char *textureFileName;
};

void
transformMesh(glm::mat4 transform, Mesh *baseMesh, Mesh *toStore)
{
    for(uint32 v = 0;
        v < baseMesh->numVerts;
        ++v)
    {
        toStore->vertices[v].pos = glm::vec3(transform * glm::vec4(baseMesh->vertices[v].pos, 1.0f));
    }
    
}
#define MESH_H
#endif
