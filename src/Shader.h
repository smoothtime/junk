#if !defined(SHADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

struct Shader
{
    GLuint program;
    Shader(char *vertexPath, char *fragmentPath);
};
#define SHADER_H
#endif
