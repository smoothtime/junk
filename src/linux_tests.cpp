/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "game.cpp"

uint32
countNodes(IDNode *start)
{
    uint32 ret = 0;
    IDNode *node = start;
    while(node)
    {
        ret++;
        node = node->next;
    }
    return ret;
}

void
testIDSystem(MemoryArena *ma)
{
    IDSystem *testIDS = initializeIDSystem(ma);

    uint32 i, id, returnedCount, freedCount;
    for(i = 1; i < Megabytes(1); ++i)
    {
        id =  testIDS->getNextID(ma);
        assert(id == i);
    }
    
    testIDS->returnID(ma, 7);
    testIDS->returnID(ma, 1024);
    assert(countNodes(testIDS->returnedList) == 2);
    assert(testIDS->getNextID(ma) == 1024);
    assert(testIDS->getNextID(ma) == 7);
    assert(testIDS->getNextID(ma) == Megabytes(1));
    assert(countNodes(testIDS->freedFreeList) == 2);

    for(i = 256; i < 1024; ++i)
    {
        testIDS->returnID(ma, i);
    }
    assert(countNodes(testIDS->returnedList) == 1024 - 256);
    for(i = 0; i < 10; ++i)
    {
        testIDS->getNextID(ma);
    }
    assert(countNodes(testIDS->freedFreeList) == 10);
    

    
}

int
main(int argc, char **argv)
{
    void *baseMemAdr = malloc(Megabytes(2));
    MemoryArena memArena;
    initializeArena(&memArena, Megabytes(2), (uint8 *)baseMemAdr);
    testIDSystem(&memArena);
}
