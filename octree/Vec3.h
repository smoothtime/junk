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
    Vec3(const Vec3& copy)
    {
        x = copy.x;
        y = copy.y;
        z = copy.z;
    }

    static void *operator new(size_t size) {
        Vec3 *result = PushStruct(&global_arena, Vec3);
        return (void *) result;
    }
    static void operator delete(void *block) { 
        //fuck
    }

    Vec3& operator+=(const Vec3& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }
                                            
};

inline Vec3
operator+(Vec3 lhs, const Vec3& rhs)
{
    Vec3 *ret = new Vec3(lhs.x + rhs.x,
                        lhs.y + rhs.y,
                        lhs.z + rhs.z);
    return *ret;
}

Vec3 Vec3ScalarMult(real32 scale, Vec3 vec)
{
    return Vec3(vec.x * scale,
                vec.y * scale,
                vec.z * scale);
}
