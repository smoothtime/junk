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

    Vec3& operator+=(const Vec3& rhs)
    {
        this.x += rhs.x;
        this.y += rhs.y;
        this.z += rhs.z;
        return *this;
    }
                                            
};

Vec3 Vec3ScalarMult(real32 scale, Vec3 vec)
{
    return Vec3(vec.x * scale,
                vec.y * scale,
                vec.z * scale);
}
