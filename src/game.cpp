/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "game.h"
#include <GLM\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GAME_UPDATE(gameUpdate)
{
    GameState *gameState = (GameState *) memory->permStorage;
    if(!memory->isSimulationInitialized)
    {
        memory->isSimulationInitialized = true;
        gameState->camera = { vec3(0.0f, 0.0f, 0.0f),
                              { -0.1f, -0100.0f, -8.0f, 8.0f, 6.0f, -6.0f},
                              identityMatrix()
        };
        gameState->testDelta = vec3(0.01f, 0.01f, 0.0f);
        initializeArena(&gameState->memArena,
                        memory->permanentStorageSize - sizeof(GameState),
                        (uint8 *)memory->permStorage + sizeof(GameState));

        Vec3 v1 = vec3(-500.0f, -500.0f, -500.0f);
        Vec3 v2 = vec3( 500.0f,  500.0f,  500.0f);
        Vec3 origin = (v1 + v2) * 5.0f;

        AABBox box1 = { v1, v2 };

        gameState->staticEntityTree = ConstructOctree(box1, 1, &gameState->memArena);

        // testing insertion
        gameState->staticEntities[1] = {1, { vec3(-500.0f, -500.0f, -500.0f), vec3(300.0f, 300.0f, 300.0f) } };
        gameState->staticEntities[2] = {2, { vec3(-450.0f, -450.0f, -450.0f), vec3(-350.0f, -350.0f, -350.0f) } };
        gameState->staticEntities[3] = {3, { vec3(-500.0f, -500.0f, -500.0f), vec3(-300.0f, -300.0f, -300.0f) } };
        gameState->staticEntities[4] = {4, { vec3(-500.0f, -500.0f, -500.0f), vec3(-300.0f, -300.0f, -300.0f) } };
        gameState->staticEntities[5] = {5, { vec3(-500.0f, -500.0f, -500.0f), vec3(-300.0f, -300.0f, -300.0f) } };
        gameState->staticEntities[6] = {6, { vec3(5.0f, 5.0f, 5.0f), vec3(30.0f, 30.0f, 30.0f) } };
        
        for(uint32 i = 1; i < 7; ++i)
        {
            gameState->staticEntityTree->insert(gameState->staticEntities + i, &gameState->memArena);
        }
        gameState->entityCount = 6;
    }
    else
    {   
        char str[256];
        uint32 collisionIndices[6] = { 0 };
        Entity test = { 0, { vec3(-10.0f, -10.0f, -10.0f), vec3(10.0f, 10.0f, 10.0f) } };
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

        AABBox ndc = { vec3(-1.0f, -1.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f) };
        AABBox entBox = { vec3(-10.5f, -10.5f, -1.5f), vec3(10.5f, 10.5f, 1.5f) };
        bool32 beforeF = doBoundsCollide(ndc, entBox);
        mat4  frustum = createPerspectiveMatrix(&gameState->camera);
        Vec4 minAdjust = mat4ByVec3(frustum, entBox.minBound);
        Vec4 maxAdjust = mat4ByVec3(frustum, entBox.maxBound);
        entBox.minBound.x = fmin(minAdjust.v3.x, maxAdjust.v3.x);
        entBox.minBound.y = fmin(minAdjust.v3.y, maxAdjust.v3.y);
        entBox.minBound.z = fmin(minAdjust.v3.z, maxAdjust.v3.z);
        entBox.maxBound.x = fmax(minAdjust.v3.x, maxAdjust.v3.x);
        entBox.maxBound.y = fmax(minAdjust.v3.y, maxAdjust.v3.y);
        entBox.maxBound.z = fmax(minAdjust.v3.z, maxAdjust.v3.z);

        Frustum fr = gameState->camera.frustum;
        glm::mat4 glmFrustum = glm::frustum(fr.l, fr.r, fr.b, fr.t, fr.n, fr.f);

        bool32 AfterF = doBoundsCollide(ndc, entBox);
        //gameState->camera.position += gameState->testDelta;
        gameState->camera.viewMatrix = translationMatrix(gameState->camera.position);

    }   
}

GAME_RENDER(gameRender)
{
    GameState *gameState = (GameState *) memory->permStorage;
    if(!memory->isRendererInitialized)
    {
        //Initialize OpenGL resources
        memory->isRendererInitialized = true;
        
        glewExperimental = GL_TRUE;
        if(glewInit() != GLEW_OK)
        {
            printf("Failed to init GLEW");
        }


        gameState->rendRefs = PushStruct(&gameState->memArena, RenderReferences);
        RenderReferences *rr = gameState->rendRefs;
        rr->maxObjects = 256;
        rr->shaders = PushArray(&gameState->memArena, rr->maxObjects, Shader);
        rr->textures = PushArray(&gameState->memArena, rr->maxObjects, GLuint);
        rr->VAOs = PushArray(&gameState->memArena, rr->maxObjects, GLuint);
        rr->VBOs = PushArray(&gameState->memArena, rr->maxObjects, GLuint);
        rr->EBOs = PushArray(&gameState->memArena, rr->maxObjects, GLuint);
        rr->modelMatrices = PushArray(&gameState->memArena, rr->maxObjects, mat4);
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

/*        mat4 modelM = identityMatrix();
        mat4 viewM = identityMatrix();//gameState->camera.viewMatrix;
        mat4 projM = identityMatrix();//createPerspectiveMatrix(&gameState->camera);
*/
        
        Frustum fr = gameState->camera.frustum;
        glm::mat4 glmFrustum = glm::frustum(fr.l, fr.r, fr.b, fr.t, fr.n, fr.f);
        glm::mat4 glmModel;
        glm::mat4 glmView;
        glm::mat4 glmProjection;
        
        glmModel = glm::rotate(glmModel, -55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        glmView = glm::translate(glmView, glm::vec3(0.0f, 0.0f, -3.0f));
        glmProjection = glm::perspective(45.0f, (GLfloat)800 / (GLfloat) 600, 0.1f, 100.0f);
        

        for(uint32 r = 0; r < rr->numObjects; ++r)
        {
            glUseProgram(rr->shaders[r].program);
            GLdouble sinTime = (sin(timeVal)) + 0.5f;
        
            GLint uniColorLoc = glGetUniformLocation(rr->shaders[r].program, "uniColor");
            glUniform4f(uniColorLoc, 0.0f, (GLfloat) sinTime, 1.0f / (GLfloat) sinTime, (GLfloat) sinTime);

            GLint modelMatrix = glGetUniformLocation(rr->shaders[r].program, "model");
            glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(glmModel));
                               // (GLfloat *) &modelM);

        
            GLint viewMatrix = glGetUniformLocation(rr->shaders[r].program, "view");
                               glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(glmView));
                               //(GLfloat *) &viewM);
        
            GLint projMatrix = glGetUniformLocation(rr->shaders[r].program, "projection");
            glUniformMatrix4fv(projMatrix, 1, GL_TRUE, glm::value_ptr(glmProjection));
                               //&projM);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            glBindVertexArray(rr->VAOs[r]);
            glBindTexture(GL_TEXTURE_2D, rr->textures[r]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);

            glDisable(GL_BLEND);
        }
        glUseProgram(0);
    }
}
