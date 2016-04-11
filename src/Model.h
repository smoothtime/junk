#if !defined(MODEL_H)
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
//    glm::vec3 colors;
    glm::vec2 texCoords;    
};

struct Model
{
    uint32 numVerts;
    Vertex *vertices;
    uint32 numIndices;
    uint32 *indices;
    char *vShaderPath;
    char *fShaderPath;
};
#define MODEL_H
#endif
