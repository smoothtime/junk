/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#include "game.h"
#include "glRender.h"
#include "hackyVisualization.h"

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

Model *
loadAndParse3ds(thread_context *thread, GameState *gameState,
          platformServiceReadEntireFile *psRF, const char *relPath,
          Model *result, bool32 isCollisionMesh)
{
    
    const uint16 CHUNK_MAIN                = 0x4d4d;
    const uint16 CHUNK_3D_EDITOR           = 0x3d3d;
    const uint16 CHUNK_OBJECT_BLOCK        = 0x4000;
    const uint16 CHUNK_TRIANGULAR_MESH     = 0x4100;
    const uint16 CHUNK_VERTICES_LIST       = 0x4110;
    const uint16 CHUNK_FACE_LIST           = 0x4120;
    const uint16 CHUNK_MAPPING_COORDINATES = 0x4140;
    const uint16 CHUNK_MATERIAL_BLOCK      = 0xafff;

    GeneralAllocator *alctr = gameState->assetAlctr;
    
    uint32 meshCount = 0;
    readMeshListEntry *first = NEW(alctr, readMeshListEntry);
    readMeshListEntry *current = first;

    Mesh *currentMesh = NEW(alctr, Mesh);
    current->entry = &currentMesh;
    
    read_file loadedFile = psRF(thread, relPath);
    // Parse file
    uint16 *chunkId;
    uint32 *chunkLength;
    uint16 *count;
    uint16 *faceFlags;
    uint8 *readP = (uint8 *)loadedFile.memory;
    while(readP < (uint8 *)loadedFile.memory + loadedFile.size)
    {
        chunkId = (uint16 *)readP;
        readP += sizeof(uint16);
        chunkLength = (uint32 *)readP;
        readP += sizeof(uint32);
        switch(*chunkId)
        {
            case CHUNK_MAIN:
                // read children
                break;
                
            case CHUNK_3D_EDITOR:
                // read children
                break;
                
            case CHUNK_OBJECT_BLOCK:
            {
                JString *name = readString(&gameState->memArena, readP);
                readP += name->size;
                break;
            }
            
            case CHUNK_TRIANGULAR_MESH:
                // read children
                if(meshCount++ > 0)
                {
                    current->next = NEW(alctr, readMeshListEntry);
                    current = current->next;
                    currentMesh = NEW(alctr, Mesh);
                    current->entry = &currentMesh; 
                }
                break;
                
            case CHUNK_VERTICES_LIST:
            {
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                currentMesh->numVerts = (uint32) *count;
                currentMesh->vertices = (Vertex *) gameState->assetAlctr->alloc(sizeof(Vertex) * currentMesh->numVerts);
                for(int32 i = 0; i < *count; ++i)
                {
                    currentMesh->vertices[i].pos = glm::vec3( *((real32 *)readP + 0),
                                                        *((real32 *)readP + 1),
                                                        *((real32 *)readP + 2)
                                                      );
                    readP += 3 * sizeof(real32);
                }
                break;
            }
            
            case CHUNK_FACE_LIST:
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                currentMesh->numIndices = (uint32)(*count * 3);
                currentMesh->indices = (uint32 *) gameState->assetAlctr->alloc(sizeof(uint32) * currentMesh->numIndices);
                for(uint32 i = 0; i < currentMesh->numIndices; i+=3)
                {
                    currentMesh->indices[i + 0] = (uint32)*((uint16 *)readP);
                    currentMesh->indices[i + 1] = (uint32)*((uint16 *)readP + 1);
                    currentMesh->indices[i + 2] = (uint32)*((uint16 *)readP + 2);
                    // pass a not important face flag
                    faceFlags = (uint16 *) readP;
                    readP += 4 * sizeof(uint16);
                }
                break;
                 
            case CHUNK_MAPPING_COORDINATES:
                count = (uint16 *)readP;
                readP += sizeof(uint16);
                for(uint32 i = 0; i < currentMesh->numVerts; ++i)
                {
                    currentMesh->vertices[i].texCoords = glm::vec2( *((real32 *)readP + 0),
                                                              *((real32 *)readP + 1)
                                                           );
                    readP += 2 * sizeof(real32);
                }
                break;
                
            case CHUNK_MATERIAL_BLOCK:
                // postponed until we get all the verts displaying correctly
                readP += (*chunkLength - 6); // -6 for the header we've read already
                break;
                
            default:
                readP += (*chunkLength - 6); // -6 for the header we've read already
        }
    }

    if(!isCollisionMesh)
    {
        result->numRenderMesh = meshCount;
        result->renderMeshes = (RenderMesh *) alctr->alloc(sizeof(Mesh) * meshCount);
        current = first;
        for(uint32 i = 0; i < meshCount; ++i)
        {
            RenderMesh *renderMesh = result->renderMeshes + i;
            renderMesh->mesh = *(current->entry);
            readMeshListEntry *tmp = current->next;
            alctr->dealloc(current);
            current = tmp;
        }
        assert(current == 0);

    }
    else
    {
        
        result->numCollisionMesh = meshCount;
        result->collisionMeshes = (CollisionMeshPair *) alctr->alloc(sizeof(CollisionMeshPair) * meshCount);
        current = first;
        for(uint32 i = 0; i < meshCount; ++i)
        {
            result->collisionMeshes[i].baseMesh = *(current->entry);
            Mesh *bm = result->collisionMeshes[i].baseMesh;
            readMeshListEntry *tmp = current->next;
            alctr->dealloc(current);
            current = tmp;

            result->collisionMeshes[i].worldMesh = NEW(alctr, Mesh);
            Mesh *wm = result->collisionMeshes[i].worldMesh;
            wm->numVerts = bm->numVerts;
            wm->numIndices = bm->numIndices;
            wm->vertices = (Vertex *) alctr->alloc(sizeof(Vertex) * bm->numVerts);
            wm->indices = (uint32 *) alctr->alloc(sizeof(uint32) * bm->numIndices);
        }
        assert(current == 0);
    }
    
#define RETARDED_CUBE_3DS 0
#if RETARDED_CUBE_3DS
    for(uint32 i = 0; i < currentMesh->numIndices; ++i)
    {
        switch(currentMesh->indices[i])
        {
            case 0:
            case 1:
                currentMesh->indices[i] = 0;
                break;
            case 2:
            case 3:
            case 4:
                currentMesh->indices[i] = 2;
                break;
            case 5:
            case 6:
            case 7:
                currentMesh->indices[i] = 5;
                break;
            case 8:
            case 9:
            case 10:
                currentMesh->indices[i] = 8;
                break;
            case 11:
                currentMesh->indices[i] = 11;
                break;
            case 12:
            case 13:
            case 14:
                currentMesh->indices[i] = 12;
                break;
            case 15:
            case 16:
            case 17:
                currentMesh->indices[i] = 15;
                break;
            case 18:
            case 19:
                currentMesh->indices[i] = 18;
                break;
            default:
                break;
                
        }
    }
#endif

    
        
    return result;
}

Model *loadModel(thread_context *thread, GameState *gameState,
          platformServiceReadEntireFile *psRF, Renderable *rendFiles)
{
    Model *result = gameState->models + gameState->numModels;
    loadAndParse3ds(thread, gameState, psRF, rendFiles->renderModelFileName, result, false);
    for(uint32 meshIdx = 0; meshIdx < result->numRenderMesh; ++meshIdx)
    {
        Mesh *m = result->renderMeshes[meshIdx].mesh;
        uint32 sIdx = initShader(gameState->rendRefs, rendFiles->vShaderFileName, rendFiles->fShaderlFileName);
        uint32 tIdx = initTexture(gameState->rendRefs, rendFiles->textureFileName);
        uint32 vIdx = initVertexIndexBuffers(gameState->rendRefs, m);
        result->renderMeshes[meshIdx].rri = { sIdx, tIdx, vIdx, vIdx, vIdx, m->numIndices };
    }
    loadAndParse3ds(thread, gameState, psRF, rendFiles->collisionModelFileName, result, true);
    gameState->numModels++;

    return result;
}

void
initializeLevel(GameState *gameState, thread_context *thread, GameMemory *memory)
{
    switch(gameState->gameLevel)
    {
        case 0:
        {
            //TODO: bundle asset file so it knows what model to pair with what shader, etc
            // hardcoded test model and texture
            Renderable testModel = {};
            testModel.renderModelFileName = "../data/test.3ds";
            testModel.collisionModelFileName = "../data/test.3ds";
            testModel.vShaderFileName = "../data/vshader_1.vs";
            testModel.fShaderlFileName = "../data/fshader_1.fs";
            testModel.textureFileName = "../data/wall.jpg";
            
            Model *model = loadModel(thread, gameState, memory->platformServiceReadFile,
                                     &testModel);


            assert(model->numRenderMesh > 0);
            assert(model->numCollisionMesh > 0);
            assert(model->renderMeshes->mesh->numVerts != 0);
            assert(model->collisionMeshes->baseMesh->numVerts != 0);
            assert(model->collisionMeshes->worldMesh->numVerts != 0);

        
            gameState->entityCount = 5;
            for(uint32 x = 0; x < 5; ++x)
            {
                Entity *ent = &gameState->dynamicEntities[x];
                ent->isStatic = true;
                ent->entityIndex = x;
                ent->position = glm::vec3(3.5f * x, 0.0f, -3.0f);
                ent->transMtx = glm::translate(glm::mat4(), ent->position);
                ent->rotMtx = glm::mat4(1);
                ent->model = model;
                model->aabb = createBaseAABBox(model->collisionMeshes[0].baseMesh);

                // for every mesh past the first, update bounds to include all meshes
                for(uint32 cmIdx = 1; cmIdx < model->numCollisionMesh; ++cmIdx)
                {
                    Mesh *m = model->collisionMeshes[cmIdx].baseMesh;
                    updateAABBox(&model->aabb, m);
                }
            }
            gameState->isLevelStarted = true;
            break;
        }
        case 1:
            break;
        default:
            assert(false);
            break;
    }
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
        // initialize color picking resources
        rr->colorPickShader = Shader("../data/colorpick_vshader.vs", "../data/colorpick_fshader.fs"); 
        glGenFramebuffers(1, &rr->colorPickFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, rr->colorPickFBO);
        glGenTextures(1, &rr->colorPickTexture);
        char log[256];
        sproot(log, "color pick texture %d", rr->colorPickTexture);
        gLog(log);
        glBindTexture(GL_TEXTURE_2D, rr->colorPickTexture);
        checkGLError(gLog);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        checkGLError(gLog);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        checkGLError(gLog);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        checkGLError(gLog);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        checkGLError(gLog);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, input->resX, input->resY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        checkGLError(gLog);
        glGenRenderbuffers(1, &rr->colorPickDepthRenderBuffer);
        checkGLError(gLog);
        glBindRenderbuffer(GL_RENDERBUFFER, rr->colorPickDepthRenderBuffer);
        checkGLError(gLog);
        //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, gameState->resWidth, gameState->resHeight);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rr->colorPickTexture, 0);
        checkGLError(gLog);
        //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rr->colorPickDepthRenderBuffer);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(status == GL_FRAMEBUFFER_COMPLETE)
        {
            gLog("lovely");
        }
        else if(status == GL_FRAMEBUFFER_UNDEFINED)
        {
            gLog("could not create color framebuffer. framebuffer undefined");
            assert(false);
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
        {
            gLog("could not create color framebuffer. incomplete attachment");
            assert(false);
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
        {
            gLog("could not create color framebuffer. missing attachment");
            assert(false);
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
        {
            gLog("could not create color framebuffer. incomplete draw buffer");
            assert(false);
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
        {
            gLog("could not create color framebuffer. incomplete read buffer");
            assert(false);
        }
        else if(status == GL_FRAMEBUFFER_UNSUPPORTED)
        {
            gLog("could not create color framebuffer. unsupported?");
            assert(false);
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
        {
            gLog("could not create color framebuffer. incomplete multisample");
            assert(false);
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
        {
            gLog("could not create color framebuffer. incomplete layer targets");
            assert(false);
        }
        

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // allocate memory for additional shaders, textures, etc
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

        gameState->isLevelStarted = false;
        gameState->gameLevel = 0;
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
        if(!gameState->isLevelStarted)
        {
            initializeLevel(gameState, thread, memory);
        }
        
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
        
        Entity *hackyVisEnt = gameState->dynamicEntities + 5;

        // Simulate
        for(uint32 x = 0;
            x < gameState->entityCount;
            x++)
        {
            Entity *ent = &gameState->dynamicEntities[x];
            if(hackyVisEnt->model != 0)
            {
                AABBox box1 = transformAABB(ent->transMtx * ent->rotMtx, &ent->model->aabb);
                AABBox box2 = hackyVisEnt->model->aabb;
                if(doBoundsCollide(box1, box2))
                {
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
            }
            // ent->rotMtx = glm::rotate(ent->rotMtx, (GLfloat)gameState->deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
            
            real32 dummy = ent->model->aabb.minBound.x;

        }
        
        // Render
        RenderReferences *rr = gameState->rendRefs;        
        if(input->newLeftClick)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, rr->colorPickFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rr->colorPickTexture, 0);
            glViewport(0, 0, input->resX, input->resY);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            renderEntities(gameState, glmView, glmProjection, true);
            glFlush();
            glFinish();
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            
            uint8 pixelColor[4];
            glReadPixels(input->mouseX, input->resY - input->mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelColor);
            if(pixelColor[0] > 0)
            {
                char log[256];
                uint32 objectID = pixelColor[0]
                                  + (pixelColor[1] * 256)
                                  + (pixelColor[2] * (256 * 256));
                sproot(log, "\nclicked on a thing with id: %d\nColors: %d, %d, %d", objectID, pixelColor[0], pixelColor[1], pixelColor[2]);
                gLog(log);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderEntities(gameState, glmView, glmProjection, false);
        
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
