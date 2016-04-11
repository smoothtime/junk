struct Vec3
{
    union {
        struct {
            real32 x;
            real32 y;
            real32 z;
        };
        real32 xyz[3];
    };                                      
};

inline Vec3
vec3(real32 x, real32 y, real32 z)
{
    Vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

Vec3& operator+=(Vec3& lhs, Vec3 rhs)
{

    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

Vec3& operator-=(Vec3& lhs, Vec3 rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

inline Vec3
operator+(Vec3 lhs, Vec3 &rhs)
{
    Vec3 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;
}

inline Vec3
operator-(Vec3 lhs, Vec3 &rhs)
{
    Vec3 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;
}

inline Vec3
operator*(Vec3 lhs, real32 scale)
{
    Vec3 result;
    result.x = lhs.x * scale;
    result.y = lhs.y * scale;
    result.z = lhs.z * scale;
    return result;
}

inline Vec3&
operator*=(Vec3& lhs, real32 scale)
{
    lhs.x *= scale;
    lhs.y *= scale;
    lhs.z *= scale;
    return lhs;
}
