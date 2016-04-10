/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "game.h"

GAME_UPDATE(gameUpdate)
{
    GameState *gameState = (GameState *) memory->permStorage;
    if(!memory->isSimulationInitialized)
    {
        memory->isSimulationInitialized = true;
        gameState->camera = { glm::vec3(0.0f, 0.0f, 0.0f),
                              { -0.1f, -0100.0f, -8.0f, 8.0f, 6.0f, -6.0f},
                              glm::mat4(1.0f)
        };
        gameState->testDelta = glm::vec3(0.01f, 0.01f, 0.0f);
        MemoryArena *memArena = &gameState->memArena;
        initializeArena(memArena,
                        memory->permanentStorageSize - sizeof(GameState),
                        (uint8 *)memory->permStorage + sizeof(GameState));

        // TODO(james): find out a good way to lay out memory so you don't have to
        // pointer chase both to the Model and then to all the elements
        gameState->maxModels = 128;
        gameState->models = PushArray(memArena, gameState->maxModels, Model);

        loadModel(thread, gameState, memory->platformServiceReadFile, "../data/model1.dae");
        
        glm::vec3 v1 = glm::vec3(-500.0f, -500.0f, -500.0f);
        glm::vec3 v2 = glm::vec3( 500.0f,  500.0f,  500.0f);
        glm::vec3 origin = (v1 + v2) * 5.0f;
        AABBox box1 = { v1, v2 };
        gameState->staticEntityTree = ConstructOctree(box1, 1, memArena);
        // testing insertion
        gameState->staticEntities[1] = {1, { glm::vec3(-500.0f, -500.0f, -500.0f), glm::vec3(300.0f, 300.0f, 300.0f) } };
        gameState->staticEntities[2] = {2, { glm::vec3(-450.0f, -450.0f, -450.0f), glm::vec3(-350.0f, -350.0f, -350.0f) } };
        gameState->staticEntities[3] = {3, { glm::vec3(-500.0f, -500.0f, -500.0f), glm::vec3(-300.0f, -300.0f, -300.0f) } };
        gameState->staticEntities[4] = {4, { glm::vec3(-500.0f, -500.0f, -500.0f), glm::vec3(-300.0f, -300.0f, -300.0f) } };
        gameState->staticEntities[5] = {5, { glm::vec3(-500.0f, -500.0f, -500.0f), glm::vec3(-300.0f, -300.0f, -300.0f) } };
        gameState->staticEntities[6] = {6, { glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(30.0f, 30.0f, 30.0f) } };       
        for(uint32 i = 1; i < 7; ++i)
        {
            gameState->staticEntityTree->insert(gameState->staticEntities + i, memArena);
        }
        gameState->entityCount = 6;
    }
    else
    {   
        char str[256];
        uint32 collisionIndices[6] = { 0 };
        Entity test = { 0, { glm::vec3(-10.0f, -10.0f, -10.0f), glm::vec3(10.0f, 10.0f, 10.0f) } };
        gameState->staticEntityTree->checkCollisions(gameState->staticEntities, &test, collisionIndices, ArrayCount(collisionIndices));
        for(uint32 i = 0; i < ArrayCount(collisionIndices) && collisionIndices[i]; ++i)
        {
            sprintf_s(str, "Collided with static entity of index %d\n", collisionIndices[i]);
            // TODO(james): expose logging in platform layer
            // memory->log(str);
        }
        
        if(gameState->camera.position.x > 1.0f || gameState->camera.position.x < -1.0f)
        {
            gameState->testDelta *= -1.0f; 
        }
        
        gameState->camera.viewMatrix = glm::translate(gameState->camera.viewMatrix, glm::vec3(0.0f));

    }   
}

GAME_RENDER(gameRender)
{
    GameState *gameState = (GameState *) memory->permStorage;
    MemoryArena *memArena = &gameState->memArena;
    if(!memory->isRendererInitialized)
    {
        //Initialize OpenGL resources
        memory->isRendererInitialized = true;
        
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
        rr->numObjects = 1;
        initObjects(rr);
        initShaders(rr);
        initTextures(rr);
    }
    else
    {
        RenderReferences *rr = gameState->rendRefs;        
        
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        Frustum fr = gameState->camera.frustum;
        glm::mat4 glmFrustum = glm::frustum(fr.l, fr.r, fr.b, fr.t, fr.n, fr.f);
        glm::mat4 glmModel;
        glm::mat4 glmView;
        glm::mat4 glmProjection;

        GLfloat sinTime = (GLfloat)(sin(timeVal)) + 0.5f;
        glmModel = glm::rotate(glmModel, sinTime, glm::vec3(1.0f, 0.0f, 0.0f));
        glmView = glm::translate(glmView, glm::vec3(0.0f, 0.0f, -3.0f));
        glmProjection = glm::perspective(45.0f, (GLfloat)800 / (GLfloat) 600, 0.1f, 100.0f);
        

//        for(uint32 r = 0; r < rr->numObjects; ++r)
//        {
        uint32 r = 0;
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rr->textures[r]);
        glUniform1i(glGetUniformLocation(rr->shaders[r].program, "ourTexture1"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, rr->textures[r+1]);
        glUniform1i(glGetUniformLocation(rr->shaders[r].program, "ourTexture2"), 1);
        
        glUseProgram(rr->shaders[r].program);

        GLint modelMatrix = glGetUniformLocation(rr->shaders[r].program, "model");
        GLint viewMatrix = glGetUniformLocation(rr->shaders[r].program, "view");
        GLint projMatrix = glGetUniformLocation(rr->shaders[r].program, "projection");
        
        glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(glmModel));
        glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(glmView));
        glUniformMatrix4fv(projMatrix, 1, GL_FALSE, glm::value_ptr(glmProjection));

//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
        glBindVertexArray(rr->VAOs[r]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

//        glDisable(GL_BLEND);
//        }
        glUseProgram(0);
    }
}
