/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "game.h"

// expects mouseY to be 0 = top resY = bottom
// pitch and yaw in radians
glm::vec3
getClickRay(real32 mouseX, uint32 resX, real32 mouseY, uint32 resY, real32 pitch, real32 yaw, glm::mat4 model, glm::mat4 projection)
{
    glm::vec3 vNear = glm::unProject(glm::vec3(mouseX, resY - mouseY, 0.0f),
                                     model, projection,
                                     glm::vec4(0.0f, 0.0f, (real32) resX, (real32) resY));
    glm::vec3 vFar = glm::unProject(glm::vec3(mouseX, resY - mouseY, 1.0f),
                                    model, projection,
                                    glm::vec4(0, 0, resX, resY));
    glm::vec3 rayDir = vFar - vNear;                                             
    rayDir = glm::normalize(rayDir);
    rayDir = glm::rotateX(rayDir, 1.0f * pitch);
    rayDir = glm::rotateY(rayDir, -1.0f * yaw);

    return rayDir;
}

extern "C"
GAME_UPDATE(gameUpdate)
{
    GameState *gameState = (GameState *) memory->permStorage;
    if(!memory->isInitialized)
    {
        memory->isInitialized = true;        
        gameState->camera = initCamera(glm::vec3(2 * 3.5f, 0.0f, 10.0f),
                                       glm::vec3(0, 0.0f, -1.0f),
                                       WORLD_UP_VECTOR);
        Camera cam = gameState->camera;
        cam.right = glm::normalize(glm::cross(cam.up, cam.direction));
        MemoryArena *memArena = &gameState->memArena;
        initializeArena(memArena,
                        memory->permanentStorageSize - sizeof(GameState),
                        (uint8 *)memory->permStorage + sizeof(GameState));
        gameState->assetAlctr = initGeneralAllocator(memArena, Megabytes(5));


        // Initialize Rendering part
        glewExperimental = GL_TRUE;
        if(glewInit() != GLEW_OK)
        {
            printf("Failed to init GLEW");
        }

        gameState->rendRefs = (RenderReferences *) gameState->assetAlctr->alloc(sizeof(RenderReferences));
        
        RenderReferences *rr = gameState->rendRefs;
        rr->maxObjects = 256;
        rr->shaders = (Shader *) gameState->assetAlctr->alloc(sizeof(Shader) * rr->maxObjects);
        rr->textures = (GLuint *) gameState->assetAlctr->alloc(sizeof(GLuint) * rr->maxObjects);
        rr->VAOs = (GLuint *) gameState->assetAlctr->alloc(sizeof(GLuint) * rr->maxObjects);
        rr->VBOs = (GLuint *) gameState->assetAlctr->alloc(sizeof(GLuint) * rr->maxObjects);
        rr->EBOs = (GLuint *) gameState->assetAlctr->alloc(sizeof(GLuint) * rr->maxObjects);

        // TODO(james): find out a good way to lay out memory so you don't have to
        // pointer chase both to the Model and then to all the elements
        gameState->maxModels = 128;
        gameState->models = (Model *) gameState->assetAlctr->alloc(sizeof(Model) * gameState->maxModels);

        // hardcoded test model and textures
        Model *model = loadModel(thread, gameState, memory->platformServiceReadFile, "../data/test.3ds");
        initShader(gameState->rendRefs, "../data/vshader_1.vs", "../data/fshader_1.fs");
        initTexture(gameState->rendRefs, "../data/wall.jpg");
        initVertexIndexBuffers(gameState->rendRefs, model);

        gameState->entityCount = 5;
        for(uint32 x = 0; x < 5; ++x)
        {
            Entity *ent = &gameState->staticEntities[x];
            ent->isStatic = true;
            ent->entityIndex = x;
            ent->position = glm::vec3(3.5f * x, 0.0f, -3.0f);
            ent->transMtx = glm::translate(glm::mat4(), ent->position);
            ent->rotMtx = glm::mat4(1);
            ent->renderInfo = { 0, 0, 0, 0, 0, model->numIndices };
            ent->aabb = createBaseAABBox(ent, model);
        }
    }
    else if(reloadExtensions)
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

        // Process input
        if(input->space)
        {
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

                
                glm::vec3 tar = cam->position + cam->direction;
                cam->right = glm::normalize(glm::cross(WORLD_UP_VECTOR, glm::normalize(cam->position - tar)));;
                cam->up = glm::normalize(glm::cross(glm::normalize(cam->position - tar), cam->right));
                
            }
       
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
        //cam->position.y = 1.0f;

        glm::mat4 glmModel;
        glm::mat4 glmView;
        glm::mat4 glmProjection;

        glmView = glm::lookAt(cam->position, cam->position + cam->direction, cam->up);
        glmProjection = glm::perspective(45.0f, (GLfloat)input->resX / (GLfloat) input->resY, 0.1f, 100.0f);

        real32 adjustedYaw = cam->yaw + (real32) PI32 / 2.0f;
        glm::vec3 rayDir = getClickRay(input->mouseX, input->resX,
                                       input->mouseY, input->resY,
                                       cam->pitch, adjustedYaw,
                                       glmModel, glmProjection);
        if(input->newLeftClick)
        {
            Model *clickVis = gameState->models + 1;
            clickVis->numVerts = 8;
            clickVis->vertices = (Vertex *) gameState->assetAlctr->alloc(sizeof(Vertex) * 8);

            clickVis->vertices[0].pos = cam->position + (-0.05f * cam->right) +  (0.05f * cam->up)  + glm::vec3(rayDir);
            clickVis->vertices[1].pos = cam->position + (0.05f * cam->right) +  (0.05f * cam->up)   + glm::vec3(rayDir);
            clickVis->vertices[2].pos = cam->position + (-0.05f * cam->right) +  (-0.05f * cam->up) + glm::vec3(rayDir);
            clickVis->vertices[3].pos = cam->position + (0.05f * cam->right) +  (-0.05f * cam->up)  + glm::vec3(rayDir);
            clickVis->vertices[4].pos = clickVis->vertices[0].pos  + 15.0f * glm::vec3(rayDir);
            clickVis->vertices[5].pos = clickVis->vertices[1].pos  + 15.0f * glm::vec3(rayDir);
            clickVis->vertices[6].pos = clickVis->vertices[2].pos  + 15.0f * glm::vec3(rayDir);
            clickVis->vertices[7].pos = clickVis->vertices[3].pos  + 15.0f * glm::vec3(rayDir);

            for(uint8 i = 0;
                i < 8;
                ++i)
            {
                clickVis->vertices[i].pos += glm::vec3(-.0f, 0.0f, 0.0f);
            }
            
            clickVis->numIndices = 36;
            clickVis->indices = (uint32 *) gameState->assetAlctr->alloc(sizeof(uint32) * 36);

            clickVis->indices[0] = 0;
            clickVis->indices[1] = 1;
            clickVis->indices[2] = 2;
            clickVis->indices[3] = 2;
            clickVis->indices[4] = 1;
            clickVis->indices[5] = 3;
            clickVis->indices[6] = 3;
            clickVis->indices[7] = 1;
            clickVis->indices[8] = 5;
            clickVis->indices[9] = 5;
            clickVis->indices[10] = 7;
            clickVis->indices[11] = 3;
            clickVis->indices[12] = 3;
            clickVis->indices[13] = 7;
            clickVis->indices[14] = 2;
            clickVis->indices[15] = 2;
            clickVis->indices[16] = 7;
            clickVis->indices[17] = 6;
            clickVis->indices[18] = 6;
            clickVis->indices[19] = 7;
            clickVis->indices[20] = 5;
            clickVis->indices[21] = 5;
            clickVis->indices[22] = 4;
            clickVis->indices[23] = 6;
            clickVis->indices[24] = 6;
            clickVis->indices[25] = 4;
            clickVis->indices[26] = 0;
            clickVis->indices[27] = 0;
            clickVis->indices[28] = 2;
            clickVis->indices[29] = 6;
            clickVis->indices[30] = 0;
            clickVis->indices[31] = 4;
            clickVis->indices[32] = 1;
            clickVis->indices[33] = 1;
            clickVis->indices[34] = 4;
            clickVis->indices[35] = 5;
            
            if(gameState->rendRefs->VAOs[1] == 0)
            {
                initVertexIndexBuffers(gameState->rendRefs, clickVis);
                gameState->staticEntities[5] = {};
                gameState->staticEntities[5].aabb = createBaseAABBox(&gameState->staticEntities[5], clickVis);
            }
            else
            {
                RenderReferenceIndex toClear = { 1, 1, 1, 1, 1 };
                overrideVertexBuffers(gameState->rendRefs, toClear, clickVis);
                gameState->staticEntities[5].aabb = createBaseAABBox(&gameState->staticEntities[5], clickVis);
            }
            AABBox what = gameState->staticEntities[5].aabb;
            assert(what.minBound.x < what.maxBound.x);
            assert(what.minBound.y < what.maxBound.y);
            assert(what.minBound.z < what.maxBound.z);
        }


        // Simulate
        for(uint32 x = 0;
            x < gameState->entityCount;
            x++)
        {
            Entity *ent = &gameState->staticEntities[x];

            AABBox box1 = transformAABB(ent->transMtx * ent->rotMtx, &ent->aabb);
            AABBox box2 = gameState->staticEntities[5].aabb;
            if(doBoundsCollide(box1, box2))
            {
                glm::vec3 delta = glm::vec3(0);

                delta = glm::vec3(0.0f, 0.0f, 0.5f) ;

                ent->position += delta;
                ent->transMtx = glm::translate(ent->transMtx, delta);
            }
            else
            {
                uint32 notColliding = 1;
            }
            // ent->rotMtx = glm::rotate(ent->rotMtx, (GLfloat)gameState->deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
            
            real32 dummy = ent->aabb.minBound.x;

        }
        
        // Render
        glEnable(GL_DEPTH_TEST);
        RenderReferences *rr = gameState->rendRefs;        
        
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        for(uint32 x = 0;
            x < gameState->entityCount;
            x++)
        {
            Entity *entity = gameState->staticEntities + x;
            glmModel = entity->transMtx * entity->rotMtx;
            GLuint shaderProgram = rr->shaders[entity->renderInfo.shaderIndex].program;
            glUseProgram(shaderProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rr->textures[entity->renderInfo.textureIndex]);
            glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture1"), 0);

            GLint modelMatrix = glGetUniformLocation(shaderProgram, "model");
            GLint viewMatrix = glGetUniformLocation(shaderProgram, "view");
            GLint projMatrix = glGetUniformLocation(shaderProgram, "projection");
        
            glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(glmModel));
            glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(glmView));
            glUniformMatrix4fv(projMatrix, 1, GL_FALSE, glm::value_ptr(glmProjection));
            
            glBindVertexArray(rr->VAOs[entity->renderInfo.VAOIndex]);
            glDrawElements(GL_TRIANGLES, entity->renderInfo.numIndices, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            //        glEnable(GL_BLEND);
            //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //        glDisable(GL_BLEND);
            glUseProgram(0);

        }
        // hacky input visualization
        if(input->leftClick || rr->VAOs[1] != 0)
        {
            glmModel = glm::mat4(1);
            glUseProgram(rr->shaders[0].program);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, rr->textures[0]);
            glUniform1i(glGetUniformLocation(rr->shaders[0].program, "ourTexture1"), 0);

            GLint modelMatrix = glGetUniformLocation(rr->shaders[0].program, "model");
            GLint viewMatrix = glGetUniformLocation(rr->shaders[0].program, "view");
            GLint projMatrix = glGetUniformLocation(rr->shaders[0].program, "projection");
        
            glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(glmModel));
            glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(glmView));
            glUniformMatrix4fv(projMatrix, 1, GL_FALSE, glm::value_ptr(glmProjection));
            
            glBindVertexArray(rr->VAOs[1]);
            //TODO(james): for now everything is a cube
            glDrawElements(GL_TRIANGLES, gameState->models[0].numIndices, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            //        glEnable(GL_BLEND);
            //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //        glDisable(GL_BLEND);
            glUseProgram(0);
        }
        
    }   
}
