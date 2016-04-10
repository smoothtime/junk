#if !defined(MATRIX_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */


struct mat4
{
    //NOTE(james): column major memory layout
    real32
    x0, y0, z0, p0,
    x1, y1, z1, p1,
    x2, y2, z2, p2,
    t0, t1, t2, w;

    /* Row Order Display is as follows
       
       x0, x1, x2, t0,
       y0, y1, y2, t1,
       z0, z1, z2, t2,
       p0, p1, p2, w
       
     */
    
};

inline mat4
identityMatrix()
{
    // Displayed here in row major order
    mat4 ret;
    ret.x0 = 1; ret.x1 = 0; ret.x2 = 0; ret.t0 = 0;
    ret.y0 = 0; ret.y1 = 1; ret.y2 = 0; ret.t1 = 0;
    ret.z0 = 0; ret.z1 = 0; ret.z2 = 1; ret.t2 = 0;
    ret.p0 = 0; ret.p1 = 0; ret.p2 = 0; ret.w  = 1;
    return ret;
}

// NOTE(james): assumes symmetrical viewing frustum
inline mat4
perspectiveMatrix(real32 n, real32 f,
                  real32 l, real32 r,
                  real32 t, real32 b)
{
    // Displayed here in row major order
    mat4 ret;
    ret.x0 = n/r; ret.x1 = 0;   ret.x2 = 0;               ret.t0 = 0;
    ret.y0 = 0;   ret.y1 = n/t; ret.y2 = 0;               ret.t1 = 0;
    ret.z0 = 0;   ret.z1 = 0;   ret.z2 = -1*(f+n)/(f-n);  ret.t2 = (-2*f*n)/(f-n);
    ret.p0 = 0;   ret.p1 = 0;   ret.p2 = -1;              ret.w  = 0;   
    return ret;
}

inline mat4
translationMatrix(Vec3 dVec)
{
    // Displayed here in row major order
    mat4 ret;
    ret.x0 = 1; ret.x1 = 0; ret.x2 = 0; ret.t0 = dVec.x;
    ret.y0 = 0; ret.y1 = 1; ret.y2 = 0; ret.t1 = dVec.y;
    ret.z0 = 0; ret.z1 = 0; ret.z2 = 1; ret.t2 = dVec.z;
    ret.p0 = 0; ret.p1 = 0; ret.p2 = 0; ret.w  = 1;
    return ret;
}

mat4
operator*(mat4 a, mat4 b)
{
    // Calculated here in row major order
    mat4 result;
    result.x0 = a.x0 * b.x0   +   a.x1 * b.y0   +   a.x2 * b.z0   +   a.t0 * b.p0;
    result.x1 = a.x0 * b.x1   +   a.x1 * b.y1   +   a.x2 * b.z1   +   a.t0 * b.p1;
    result.x2 = a.x0 * b.x2   +   a.x1 * b.y2   +   a.x2 * b.z2   +   a.t0 * b.p2;
    result.t0 = a.x0 * b.t0   +   a.x1 * b.t1   +   a.x2 * b.t2   +   a.t0 * b.w ;

    result.y0 = a.y0 * b.x0   +   a.y1 * b.y0   +   a.y2 * b.z0   +   a.t1 * b.p0;
    result.y1 = a.y0 * b.x1   +   a.y1 * b.y1   +   a.y2 * b.z1   +   a.t1 * b.p1;
    result.y2 = a.y0 * b.x2   +   a.y1 * b.y2   +   a.y2 * b.z2   +   a.t1 * b.p2;
    result.t1 = a.y0 * b.t0   +   a.y1 * b.t1   +   a.y2 * b.t2   +   a.t1 * b.w ;

    result.z0 = a.z0 * b.x0   +   a.z1 * b.y0   +   a.z2 * b.z0   +   a.t2 * b.p0;
    result.z1 = a.z0 * b.x1   +   a.z1 * b.y1   +   a.z2 * b.z1   +   a.t2 * b.p1;
    result.z2 = a.z0 * b.x2   +   a.z1 * b.y2   +   a.z2 * b.z2   +   a.t2 * b.p2;
    result.t2 = a.z0 * b.t0   +   a.z1 * b.t1   +   a.z2 * b.t2   +   a.t2 * b.w ;
    
    result.p0 = a.p0 * b.x0   +   a.p1 * b.y0   +   a.p2 * b.z0   +   a.w  * b.p0;
    result.p1 = a.p0 * b.x1   +   a.p1 * b.y1   +   a.p2 * b.z1   +   a.w  * b.p1;
    result.p2 = a.p0 * b.x2   +   a.p1 * b.y2   +   a.p2 * b.z2   +   a.w  * b.p2;
    result.w  = a.p0 * b.t0   +   a.p1 * b.t1   +   a.p2 * b.t2   +   a.w  * b.w ;

    return result;
}

inline Vec4
mat4ByVec3(mat4 mat, Vec3 vec)
{
    Vec4 result;
    result.v3.x = mat.x0 * vec.x   +   mat.x1 * vec.y   +   mat.x2 * vec.z   +   mat.t0 * 1;
    result.v3.y = mat.y0 * vec.x   +   mat.y1 * vec.y   +   mat.y2 * vec.z   +   mat.t1 * 1;
    result.v3.z = mat.z0 * vec.x   +   mat.z1 * vec.y   +   mat.z2 * vec.z   +   mat.t2 * 1;
    result.w    = mat.p0 * vec.x   +   mat.p1 * vec.y   +   mat.p2 * vec.z   +   mat.w  * 1;
    return result;
}
#define MATRIX_H
#endif
