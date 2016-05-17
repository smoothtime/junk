/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
void inline
checkShaderCompilation(GLuint shader)
{
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Failed to compile shader:\n%s\n", infoLog);
    }
}

void inline
checkShaderProgramCompilation(GLuint program)
{
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        printf("Failed to compile shader program:\n%s\n", infoLog);
    }
}

#include "Shader.h"
Shader::Shader(const char *vertexPath, const char *fragmentPath)
{
    std::string vCode;
    std::string fCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vCode = vShaderStream.str();
    fCode = fShaderStream.str();

    const GLchar *vShaderCode = vCode.c_str();
    const GLchar *fShaderCode = fCode.c_str();

    GLuint vShader, fShader;

    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderCode, NULL);
    glCompileShader(vShader);
    checkShaderCompilation(vShader);
    
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(fShader);
    checkShaderCompilation(fShader);

    this->program = glCreateProgram();
    glAttachShader(this->program, vShader);
    glAttachShader(this->program, fShader);
    glLinkProgram(this->program);
    checkShaderProgramCompilation(this->program);

    glDeleteShader(vShader);
    glDeleteShader(fShader);

}
