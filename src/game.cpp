/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "game.h"

extern "C"
GAME_UPDATE(gameUpdate)
{
    GameState *gameState = (GameState *) memory->permStorage;
    if(!memory->isInitialized)
    {
        memory->isInitialized = true;
        gameState->camera = { glm::vec3(0.0f, 0.0f, 0.0f),
                              { -0.1f, -0100.0f, -8.0f, 8.0f, 6.0f, -6.0f},
                              glm::mat4(1.0f)
        };
        gameState->testDelta = glm::vec3(0.01f, 0.01f, 0.0f);
        MemoryArena *memArena = &gameState->memArena;
        initializeArena(memArena,
                        memory->permanentStorageSize - sizeof(GameState),
                        (uint8 *)memory->permStorage + sizeof(GameState));


        // Initialize Rendering part
        glewExperimental = GL_TRUE;
        if(glewInit() != GLEW_OK)
        {
            printf("Failed to init GLEW");
        }

        gameState->rendRefs = PushStruct(memArena, RenderReferences);
        RenderReferences *rr = gameState->rendRefs;
        rr->maxObjects = 256;
        rr->shaders = PushArray(memArena, rr->maxObjects, Shader);
        rr->textures = PushArray(memArena, rr->maxObjects, GLuint);
        rr->VAOs = PushArray(memArena, rr->maxObjects, GLuint);
        rr->VBOs = PushArray(memArena, rr->maxObjects, GLuint);
        rr->EBOs = PushArray(memArena, rr->maxObjects, GLuint);
        rr->modelMatrices = PushArray(memArena, rr->maxObjects, glm::mat4);
        rr->numObjects = 0;

        // TODO(james): find out a good way to lay out memory so you don't have to
        // pointer chase both to the Model and then to all the elements
        gameState->maxModels = 128;
        gameState->models = PushArray(memArena, gameState->maxModels, Model);
        Model *model = loadModel(thread, gameState, memory->platformServiceReadFile, "../data/plane.3ds");
        initShader(gameState->rendRefs, "../data/vshader_1.vs", "../data/fshader_1.fs");
        initTexture(gameState->rendRefs, "../data/wall.jpg");
        initObject(gameState->rendRefs, model);
        gameState->rendRefs->numObjects++;
        
    }
    else if(hack)
    {
        glewExperimental = GL_TRUE;
        if(glewInit() != GLEW_OK)
        {
            printf("Failed to init GLEW");
        }
    }
    else
    {   

        // Render
        RenderReferences *rr = gameState->rendRefs;        
        
        glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 glmModel;
        glm::mat4 glmView;
        glm::mat4 glmProjection;

        GLfloat sinTime = (GLfloat)(sin(timeVal) / 2.0f );
        glmModel = glm::rotate(glmModel, sinTime, glm::vec3(1.0f, 0.0f, 0.0f));
#if TRAP_HACK
        glmView = glm::translate(glmView, glm::vec3(0.0f, 0.0f, -1.25f));
#else
        glmView = glm::translate(glmView, glm::vec3(1.0f, -3.5f, -9.25f));
#endif
        glmProjection = glm::perspective(45.0f, (GLfloat)800 / (GLfloat) 600, 0.1f, 100.0f);
    
        for(uint32 r = 0;
            r < rr->numObjects;
            ++r)
        {
            glUseProgram(rr->shaders[r].program);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rr->textures[r]);
            glUniform1i(glGetUniformLocation(rr->shaders[r].program, "ourTexture1"), 0);

            GLint modelMatrix = glGetUniformLocation(rr->shaders[r].program, "model");
            GLint viewMatrix = glGetUniformLocation(rr->shaders[r].program, "view");
            GLint projMatrix = glGetUniformLocation(rr->shaders[r].program, "projection");
        
            glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(glmModel));
            glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(glmView));
            glUniformMatrix4fv(projMatrix, 1, GL_FALSE, glm::value_ptr(glmProjection));
            
            glBindVertexArray(rr->VAOs[r]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            //        glEnable(GL_BLEND);
            //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //        glDisable(GL_BLEND);
            glUseProgram(0);
        }    
    }   
}
