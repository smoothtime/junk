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
        gameState->camera = initCamera(glm::vec3(2 * 3.5f, 0.0f, 10.0f),
                                       glm::vec3(0, 0.0f, -1.0f),
                                       WORLD_UP_VECTOR,
                                       glm::mat4(1.0f));
        Camera cam = gameState->camera;
        cam.right = glm::normalize(glm::cross(cam.up, cam.direction));
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
        Model *model = loadModel(thread, gameState, memory->platformServiceReadFile, "../data/test.3ds");
        initShader(gameState->rendRefs, "../data/vshader_1.vs", "../data/fshader_1.fs");
        initTexture(gameState->rendRefs, "../data/wall.jpg");
        initObject(gameState->rendRefs, model);
        gameState->rendRefs->numObjects++;

        gameState->rendRefs->numObjects = 5;
        gameState->rendRefs->modelMatrices = PushArray(memArena, 5, glm::mat4);
        for(uint32 x = 0; x < 5; ++x)
        {
            gameState->rendRefs->modelMatrices[x] = glm::translate(glm::mat4(), glm::vec3(3.5f * x, 0.0f, -3.0f));
        }
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
        gameState->deltaTime = deltaTime;
        real32 sensitivity = 0.001f;
        real32 camSpeed = 10.0f * (real32) deltaTime;
        
        Camera *cam = &gameState->camera;
        if(input->mouseDeltaX || input->mouseDeltaY)
        {
            cam->yaw += input->mouseDeltaX * sensitivity;
            cam->pitch += input->mouseDeltaY * sensitivity;
            if(cam->pitch > PI32 / 2.05f)
            {
                cam->pitch = PI32 / 2.05f;
            }
            if(cam->pitch < PI32 / -2.05f)
            {
                cam->pitch = PI32 / -2.05f;
            }
            glm::vec3 camFace;
            camFace.x = cos(cam->yaw) * cos(cam->pitch);
            camFace.y = sin(cam->pitch);
            camFace.z = sin(cam->yaw) * cos(cam->pitch);
        cam->direction = glm::normalize(camFace);
        }
        
        if(input->w)
        {
            cam->position += camSpeed * cam->direction;
        }
        if(input->s)
        {
            cam->position -= camSpeed * cam->direction;
        }
        if(input->a)
        {
            cam->position -= camSpeed * glm::normalize(glm::cross(cam->direction, cam->up));
        }
        if(input->d)
        {
            cam->position += camSpeed * glm::normalize(glm::cross(cam->direction, cam->up));
        }
        cam->position.y = 1.0f;

        // Render
        glEnable(GL_DEPTH_TEST);
        RenderReferences *rr = gameState->rendRefs;        
        
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 glmModel;
        glm::mat4 glmView;
        glm::mat4 glmProjection;

        glmView = glm::lookAt(cam->position, cam->position + cam->direction, cam->up);//glm::translate(glm::mat4(), glm::vec3(-3.5f * 2, 0.0f, -10.25f));
        glmProjection = glm::perspective(45.0f, (GLfloat)800 / (GLfloat) 600, 0.1f, 100.0f);
    
        for(uint32 r = 0, x = 0;
            x < rr->numObjects;
            x++)
        {
            glmModel = rr->modelMatrices[x];
            rr->modelMatrices[x] = glm::rotate(glmModel, (GLfloat)gameState->deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
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
            //TODO(james): for now everything is a cube
            glDrawElements(GL_TRIANGLES, gameState->models[0].numIndices, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            //        glEnable(GL_BLEND);
            //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //        glDisable(GL_BLEND);
            glUseProgram(0);
            glmModel = glm::translate(glmModel, glm::vec3(1.0f, 1.1f, -0.1f));
        }    
    }   
}
