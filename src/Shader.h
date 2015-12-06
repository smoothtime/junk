#if !defined(SHADER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define SHADER_H

#include <GL/glew.h>

struct Shader
{
    GLuint program;
    Shader(char *vertexPath, char *fragmentPath);
    void use();
};

#endif
