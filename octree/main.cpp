#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#if __has_builtin(__builtin_trap)
#define Assert(Expression) if(!(Expression)) { __builtin_trap(); }
#endif

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include "MemoryArena.h"
static MemoryArena global_arena = initializeArena(8388608);
#include "Vec3.h"
#include "Entity.h"
#include "OneTypeMemoryArena.h"
static OneTypeMemoryArena entity_arena = initializeOneTypeArena(sizeof(Entity), 16);
#include "Octree.h"


void printVector(Vec3 vec, int32 child)
{
    printf("%+2.4f %+2.4f %+2.4f: child: %d\n", vec.x, vec.y, vec.z, child);
}

void initEntities()
{
    Entity *element = PushArray(&entity_arena, 8, Entity);
    new(element++) Entity(1, -8.1f, -8.1f, -8.1f, -8.0f, -8.0f, -8.0f);
    new(element++) Entity(1, 8.0f, -8.1f, -8.1f, 8.1f, -8.0f, -8.0f);

}

int main(int argc, char **argv)
{
    initEntities();
    printf("Hello\n");
    Vec3 *origin = new Vec3(0.0f, 0.0f, 0.0f);
    Vec3 *half_dim = new Vec3(10.0f, 10.0f, 10.0f);
    //Octree *children[8] = { NULL };
    uint32 entityIds = {0};
    uint32 entityCount = 0;
    Octree tree = { *origin, *half_dim,
                    0, 
                    entityIds, entityCount };
    Vec3 *zero = new Vec3(0.0f, 0.0f, 0.0f);
   
    Vec3 *lbb = new Vec3(-8.0f, -8.0f, -8.0f); 
    Vec3 *rbb = new Vec3(8.0f, -8.0f, -8.0f);
    Vec3 *ltb = new Vec3(-8.0f, 8.0f, -8.0f);
    Vec3 *rtb = new Vec3(8.0f, 8.0f, -8.0f);
   
    Vec3 *lbf = new Vec3(-8.0f, -8.0f, 8.0f);
    Vec3 *rbf = new Vec3(8.0f, -8.0f, 8.0f);
    Vec3 *ltf = new Vec3(-8.0f, 8.0f, 8.0f);
    Vec3 *rtf = new Vec3(8.0f, 8.0f, 8.0f);
   
    printVector(*zero, whichChild(zero, &tree));

    printf("\n");
   
    printVector(*lbb, whichChild(lbb, &tree));
    printVector(*rbb, whichChild(rbb, &tree));
    printVector(*ltb, whichChild(ltb, &tree));
    printVector(*rtb, whichChild(rtb, &tree));

    printf("\n");
   
    printVector(*lbf, whichChild(lbf, &tree));
    printVector(*rbf, whichChild(rbf, &tree));
    printVector(*ltf, whichChild(ltf, &tree));
    printVector(*rtf, whichChild(rtf, &tree));

    insert(1, lbb, &tree);
    insert(2, rbb, &tree);
    insert(3, ltb, &tree);
    insert(4, rtb, &tree);

    insert(5, lbf, &tree);
    insert(6, rbf, &tree);
    insert(7, ltf, &tree);
    insert(8, rtf, &tree);

    return 0;
}
