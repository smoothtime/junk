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

    static void *operator new(size_t size)
    {
        // Mainly want to be constructing vectors
        // with placement new with Allocator provided
        // memory addresses, but if we need to allocate
        // a vector on the fly, just make one on the global allocator
        return GlobalAllocator()->allocate(size);
        
    }

    static void operator delete(void *block)
    {
        GlobalAllocator()->deallocate(block);
    }

    Vec3& operator+=(const Vec3& rhs)
    {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& rhs)
    {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }
                                            
};

inline Vec3
operator+(const Vec3 &lhs, const Vec3 &rhs)
{
    return Vec3(lhs.x + rhs.x,
                lhs.y + rhs.y,
                lhs.z + rhs.z);
}

inline Vec3
operator-(const Vec3 &lhs, const Vec3 &rhs)
{
    return Vec3(lhs.x - rhs.x,
                lhs.y - rhs.y,
                lhs.z - rhs.z);
}

inline Vec3
operator*(const Vec3 &lhs, const real32 &scale)
{   
    return Vec3(lhs.x * scale,
                lhs.y * scale,
                lhs.z * scale);
}
