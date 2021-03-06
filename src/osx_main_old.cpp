#include "platform.h"

#if __has_builtin(__builtin_trap)
#define Assert(Expression) if(!(Expression)) { __builtin_trap(); }
#endif

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <cassert>
#include "allocator.h"
#include "mallocallocator.h"
#include "globalallocator.h"
#include "Vec3.h"
#include "Entity.h"
#include "jvector.h"
#include "jset.h"
#include "Octree.cpp"
#include "radixsort.h"

static junk::JVector<Entity> entities(GlobalAllocator());

void printVector(Vec3 vec, int32 child)
{
    printf("%+2.4f %+2.4f %+2.4f: child: %d\n", vec.x, vec.y, vec.z, child);
}

int main(int argc, char **argv)
{
    printf("Hello\n");
    
    Vec3 vec1(-0.5f, -0.5f, -0.5f);
    Vec3 vec2( 0.5f,  0.5f,  0.5f);
    Vec3 origin = vec1 + ((vec2 - vec1) * 0.5f); 
    
    MallocAllocator mallocator;

    junk::JVector<uint32> integers(4, &mallocator);
    integers.push_back(0);
    integers.push_back(1);
    integers.push_back(2);
    integers.push_back(3);
    integers.push_back(4);
    
    
    AABBox box1(vec1, vec2);
    AABBox box2(0.01f, 0.01f, 0.01f, 0.05f, 0.05f, 0.05f);
    
    entities.push_back(Entity(1, box2));
    
    entities.push_back(Entity(2, AABBox(box2._min * 2.0f, box2._max * 2.0)));
    entities.push_back(Entity(3, AABBox(box2._min * 3.0f, box2._max * 3.0)));
    entities.push_back(Entity(4, AABBox(box2._min * 4.0f, box2._max * 4.0)));
    entities.push_back(Entity(5, AABBox(box2._min * 5.0f, box2._max * 5.0)));

    Octree tree(&mallocator, origin, AABBox(vec1, vec2), &entities);
    for(uint32 i = 0; i < entities.size(); ++i)
    {
        tree.insert(i);
    }
    
    printf("Hello\n");
    return 0;
}

void
jvectortest(Allocator *mallocator)
{
    junk::JVector<int32> *test1 = new junk::JVector<int32>(mallocator);
    test1->push_back(4);
    test1->push_back(2);
    test1->push_back(1);
    test1->push_back(0);
    test1->push_back(-1);
    int32 four = (*test1)[0];
    int32 first = *test1->begin();
    int32 last = *test1->end();
    size_t length = test1->size();
    test1->resize(5);
}
