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
        gLog = memory->log;

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
        // pointer chase both to the Mesh and then to all the elements
        gameState->maxModels = 128;
        gameState->models = (Model *) gameState->assetAlctr->alloc(sizeof(Model) * gameState->maxModels);

        // hardcoded test model and textures
        Model *model = loadModel(thread, gameState, memory->platformServiceReadFile, "../data/cube.3ds", false);
        model = loadModel(thread, gameState, memory->platformServiceReadFile, "../data/cube.3ds", true);
        gameState->numModels++;

        assert(model->numRenderMesh > 0);
        assert(model->numCollisionMesh > 0);
        assert(model->renderMeshes->mesh->numVerts != 0);
        assert(model->collisionMeshes->baseMesh->numVerts != 0);
        assert(model->collisionMeshes->worldMesh->numVerts != 0);

        for(uint32 meshIdx = 0; meshIdx < model->numRenderMesh; ++meshIdx)
        {
            Mesh *m = model->renderMeshes[meshIdx].mesh;
            uint32 sIdx = initShader(gameState->rendRefs, "../data/vshader_1.vs", "../data/fshader_1.fs");
            uint32 tIdx = initTexture(gameState->rendRefs, "../data/wall.jpg");
            uint32 vIdx = initVertexIndexBuffers(gameState->rendRefs, m);
            model->renderMeshes[meshIdx].rri = { sIdx, tIdx, vIdx, vIdx, vIdx, m->numIndices };
        }
        
        gameState->entityCount = 5;
        for(uint32 x = 0; x < 5; ++x)
        {
            Entity *ent = &gameState->staticEntities[x];
            ent->isStatic = true;
            ent->entityIndex = x;
            ent->position = glm::vec3(3.5f * x, 0.0f, -3.0f);
            ent->transMtx = glm::translate(glm::mat4(), ent->position);
            ent->rotMtx = glm::mat4(1);
            ent->model = model;

            for(uint32 cmIdx = 0; cmIdx < model->numCollisionMesh; ++cmIdx)
            {
                Mesh *m = model->collisionMeshes[cmIdx].baseMesh;
                // TODO(james) IMPORTANT(james): make an AABB by looking over all meshes in model
                model->aabb = createBaseAABBox(m);
            }
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
        gLog("?\n");
        
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
            initHackyVisModel(gameState, cam, rayDir);
        }
        
        Entity *hackyVisEnt = gameState->staticEntities + 5;

        // Simulate
        for(uint32 x = 0;
            x < gameState->entityCount;
            x++)
        {
            Entity *ent = &gameState->staticEntities[x];
            if(hackyVisEnt->model != 0)// && doBoundsCollide(box1, box2))
            {
                AABBox box1 = transformAABB(ent->transMtx * ent->rotMtx, &ent->model->aabb);
                AABBox box2 = hackyVisEnt->model->aabb;
                for(uint32 meshIdx = 0;
                    meshIdx < ent->model->numCollisionMesh;
                    ++meshIdx)
                {
                    CollisionMeshPair cmp = ent->model->collisionMeshes[meshIdx];
                    transformMesh(ent->transMtx, cmp.baseMesh, cmp.worldMesh);
                    if(genericGJK(cmp.worldMesh, hackyVisEnt->model->collisionMeshes->baseMesh))
                    {

                        glm::vec3 delta = glm::vec3(0.0f, 0.0f, 0.01f) ;

                        ent->position += delta;
                        ent->transMtx = glm::translate(ent->transMtx, delta);
                        break;
                    }
                    else
                    {
                        uint32 notColliding = 1;
                    }
                }
            }
            // ent->rotMtx = glm::rotate(ent->rotMtx, (GLfloat)gameState->deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
            
            real32 dummy = ent->model->aabb.minBound.x;

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
            for(uint32 m = 0;
                m < entity->model->numRenderMesh;
                ++m)
            {
                RenderMesh *rm = entity->model->renderMeshes + m;
                GLuint shaderProgram = rr->shaders[rm->rri.shaderIndex].program;
                glUseProgram(shaderProgram);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, rr->textures[rm->rri.textureIndex]);
                glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture1"), 0);

                GLint modelMatrix = glGetUniformLocation(shaderProgram, "model");
                GLint viewMatrix = glGetUniformLocation(shaderProgram, "view");
                GLint projMatrix = glGetUniformLocation(shaderProgram, "projection");
        
                glUniformMatrix4fv(modelMatrix, 1, GL_FALSE, glm::value_ptr(glmModel));
                glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(glmView));
                glUniformMatrix4fv(projMatrix, 1, GL_FALSE, glm::value_ptr(glmProjection));
            
                glBindVertexArray(rr->VAOs[rm->rri.VAOIndex]);
                glDrawElements(GL_TRIANGLES, rm->rri.numIndices, GL_UNSIGNED_INT, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                //        glEnable(GL_BLEND);
                //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                //        glDisable(GL_BLEND);
                glUseProgram(0);
            }

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
            
            glDrawElements(GL_TRIANGLES, gameState->models[1].renderMeshes->mesh->numIndices, GL_UNSIGNED_INT, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
            //        glEnable(GL_BLEND);
            //        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //        glDisable(GL_BLEND);
            glUseProgram(0);
        }
        
    }   
}
