#if !defined(GJK_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define SUPPORT(b1, b2, d) foo(b1, d) - bar(b2, -1.0f * d)

glm::vec3 origin = glm::vec3(0);

glm::vec3
generalSupport(ConvexHull *hull, glm::vec3 direction)
{
    glm::vec3 result;
    return result;
}

bool32
doSimplex(int32 *numPoints, glm::vec3 *points, glm::vec3 *dir)
{
    bool32 result = false;
    switch(*numPoints)
    {
        case 0:
        case 1:
            // shouldn't be able to get here
            assert(false);
            break;
        case 2:
        {

            checkABLineSegment:
            glm::vec3 A = points[0];
            glm::vec3 B = points[1];
            glm::vec3 AB = B - A;
            glm::vec3 AO = origin - A;

            if(glm::dot(AO, AB) >= 0)
            {
                // new simplex is a line segment
                points[2] = B;
                points[1] = A;
                *numPoints = 3;
                *dir = glm::cross(glm::cross(AB, AO), AB);
                break;
            }
            else
            {
                // new simplex is single point
                points[1] = A;
                *numPoints = 2;
                *dir = AO;
                break;
            }
        }
        case 3:
        {
            checkABCTriangle:
            glm::vec3 C = points[2];
            glm::vec3 B = points[1];
            glm::vec3 A = points[0];

            glm::vec3 AO = origin - A;
            glm::vec3 AC = C - A;
            glm::vec3 AB = B - A;
            glm::vec3 ABC = glm::cross(AB, AC);

            if(glm::dot(glm::cross(ABC, AC), AO) >= 0)
            {
                if(glm::dot(AC, AO) >= 0)
                {
                    // new simplex is a line segment
                    points[2] = C;
                    points[1] = A;
                    *numPoints = 3;
                    *dir = glm::cross(glm::cross(AC, AO), AC);
                    break;
                }
                else
                {
                    goto checkABLineSegment;
                }
            }
            else
            {
                // ABC X AC dot AO < 0
                if(glm::dot(glm::cross(AB, ABC), AO) > 0)
                {
                    // Star 
                    goto checkABLineSegment;
                }
                else
                {
                    // Above or below part of plane defined by triangle
                    if(glm::dot(ABC, AO) > 0)
                    {
                        points[3] = C;
                        points[2] = B;
                        points[1] = A;
                        *numPoints = 4;
                        *dir = ABC;
                        break;
                    }
                    else
                    {
                        points[3] = B;
                        points[2] = C;
                        points[1] = A;
                        *numPoints = 4;
                        *dir = -1.0f * ABC;
                        break;
                    }
                }
            }
            break;
        }
        case 4:
        {
            // We're at a tetrahedron
            glm::vec3 D = points[3];
            glm::vec3 C = points[2];
            glm::vec3 B = points[1];
            glm::vec3 A = points[0];
            glm::vec3 AB = B - A;
            glm::vec3 AC = C - A;
            glm::vec3 AD = D - A;
            glm::vec3 AO = origin - A;
            // test the planes to see if we're behind all of them
            // if behind all, then the tetrahdron contains the origin
            
            glm::vec3 ABC = glm::cross(AB, AC);
            glm::vec3 ACD = glm::cross(AC, AD);
            glm::vec3 ADB = glm::cross(AD, AB);

            bool32 aboveABC = glm::dot(ABC, AO) >= 0;
            bool32 aboveACD = glm::dot(ACD, AO) >= 0;
            bool32 aboveADB = glm::dot(ADB, AO) >= 0;
            if(aboveABC)
            {
                if(aboveACD)
                {
                    if(aboveADB)
                    {
                        //check A, 3 edges
                    }
                }
                else if(aboveADB)
                {
                }
                else
                {
                    // check triangle ABC
                    goto checkABCTriangle;
                }
                
            }
            else if(aboveACD)
            {
                if(aboveADB)
                {
                }
                else
                {
                    // check triangle ACD
                    points[1] = C;
                    points[2] = D;
                    goto checkABCTriangle;
                }
            }
            else if(aboveADB)
            {
                // check triangle ADB
                points[1] = D;
                points[2] = B;
                goto checkABCTriangle;
            }
            else
            {
                result = true;
            }
            break;
        }
    }
    return result;
}

bool32
gjk(glm::vec3 (*foo)(ConvexHull*, glm::vec3),
    glm::vec3 (*bar)(ConvexHull*, glm::vec3),
    ConvexHull *body1, ConvexHull *body2)
{
    bool32 x = false;

    glm::vec3 arbitraryDir = body1->vertices[0];
    glm::vec3 direction = foo(body1, arbitraryDir) - bar(body2, -1.0f * arbitraryDir);

    int32 numPoints = 1;
    glm::vec3 points[4] = {};
    points[1] = -1.0f * direction; //origin to initial direction

    for(;;)
    {
        glm::vec3 A = SUPPORT(body1, body2, direction);
        if(glm::dot(A, direction) < 0)
        {
            // vector from last point to A not going towards origin
            // no intersection
            break;
        }
        points[0] = A;
        if(doSimplex(&numPoints, points, &direction))
        {
            x = true;
            break;
        }
    }
    
    return x;
}

void
testGJK()
{
    ConvexHull x = {};
    bool32 test = gjk(&generalSupport, &generalSupport, &x, &x);
}


#define GJK_H
#endif
