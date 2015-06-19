struct Vec3;

struct Vec3
{
    union {
        struct {
            float x;
            float y;
            float z;
        };
        float xyz[3];
    };
    Vec3(real32 x_, real32 y_, real32 z_) : x(x_), y(y_), z(z_) {}

    static void *operator new(size_t size) {
        Vec3 *result = PushStruct(&global_arena, Vec3);
        return (void *) result;
    }
    static void operator delete(void *block) { 
        //fuck
    }
                                            
};
