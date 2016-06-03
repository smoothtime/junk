#if !defined(GJK_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: James Wells $
   $Notice: (C) Copyright 2015 by Extreme, Inc. All Rights Reserved. $
   ======================================================================== */

#define SUPPORT(b1, b2, d) foo(b1, d) - bar(b2, -1.0f * d)
const uint32 BF_ABC = 0x01;
const uint32 BF_ACD = 0x02;
const uint32 BF_ADB = 0x04;

glm::vec3 origin = glm::vec3(0);

glm::vec3
generalSupport(ConvexHull *hull, glm::vec3 direction)
{
    glm::vec3 result;

    result = hull->vertices[0];
    real32 maxDot = glm::dot(direction, result);
    for(uint32 vert = 1; vert < hull->numVerts; ++vert)
    {
        real32 testDot = glm::dot(hull->vertices[vert], direction);
        if(testDot > maxDot)
        {
            maxDot = testDot;
            result = hull->vertices[vert];
        }
        
    }
    return result;
}

// checks line segment specified by first two points in points
inline void
checkLineSegmentSimplex(int32 *nextIterPoints, glm::vec3 *points, glm::vec3 *dir)
{
    glm::vec3 A = points[0];
    glm::vec3 B = points[1];
    glm::vec3 AO = -1.0f * A;
    glm::vec3 AB = B - A;

    if(glm::dot(AB, AO) >= 0)
    {
        points[1] = A;
        points[2] = B;
        *nextIterPoints = 3;
        *dir = glm::cross(glm::cross(AB, AO), AB);
    }
    else
    {
        *nextIterPoints = 2;
        points[1] = A;
        *dir = AO;
    }
}

// checks triangle specified by first 3 points in points
inline void
checkTriangleSimplex(int32 *nextIterPoints, glm::vec3 *points, glm::vec3 *dir)
{
    glm::vec3 C = points[2];
    glm::vec3 B = points[1];
    glm::vec3 A = points[0];

    glm::vec3 AO = -1.0f * A;
    glm::vec3 AC = C - A;
    glm::vec3 AB = B - A;
    glm::vec3 ABC = glm::cross(AB, AC);

    if(glm::dot(glm::cross(AB, ABC), AO) > 0)
    {
        if(glm::dot(AB, AO) >= 0)
        {
            points[1] = A;
            points[2] = B;
            *nextIterPoints = 3;
            *dir = glm::cross(glm::cross(AB, AO), AB);
        }
        else
        {
            points[1] = C;
            checkLineSegmentSimplex(nextIterPoints, points, dir);
        }
    }
    else
    {
        if(glm::dot(glm::cross(ABC, AC), AO) >= 0)
        {
            points[1] = C;
            checkLineSegmentSimplex(nextIterPoints, points, dir);
        }
        else
        {
            // Inside triangle
            if(glm::dot(ABC, AO) > 0)
            {
                points[3] = C;
                points[2] = B;
                points[1] = A;
                *nextIterPoints = 4;
                *dir = ABC;
            }
            else
            {
                points[3] = B;
                points[2] = C;
                points[1] = A;
                *nextIterPoints = 4;
                *dir = -1.0f * ABC;
            }
        }
    }    
}

// My understanding of 2 Face case
/*
    if(aboveTLviaTLM)
    {
        canBe(TL, L);
        checkForTLPointTO;
    }
    else(belowTLviaTLM)
    {
        if(aboveTMviaTLM)
        {
            canBe(outfaceofTLM);
        }
        else(belowTMviaTLM)
        {
            if(aboveTMviaTMR)
            {
                canBe(TM, T);
                checkForTMPointTO;
            }
            if(belowTMviaTMR and aboveTRviaTMR)
            {
                canBe(outfaceofTMR);
            }
            if(belowTMviaTMR and belowTRviaTMR)
            {
                canBe(TR, T);
                checkForTRPointTO;
            }
        }

    }
*/
inline void
checkTwoFacesSimplex(int32 *nextIterPoints, glm::vec3 *points, glm::vec3 *dir,
                     uint32 tIndex, uint32 lIndex, uint32 mIndex, uint32 rIndex)
{
    glm::vec3 T = points[tIndex];
    glm::vec3 L = points[lIndex];
    glm::vec3 M = points[mIndex];
    glm::vec3 R = points[rIndex];

    glm::vec3 TO = -1.0f * T;
    glm::vec3 TL = L - T;
    glm::vec3 TM = M - T;
    glm::vec3 TR = R - T;
    glm::vec3 TLM = glm::cross(TL, TM);
    glm::vec3 TMR = glm::cross(TR, TM);

    if(glm::dot(glm::cross(TL, TLM), TO) > 0)
    {
        // "above"TLviaTLM
        // can be TL, T, or TM
        if(glm::dot(TL, TO) >= 0)
        {
            *dir = glm::cross(glm::cross(TL, TO), TL);
            *nextIterPoints = 3;
            points[1] = T;
            points[2] = L;
        }
        else
        {
            if(glm::dot(TM, TO))
            {
                *dir = glm::cross(glm::cross(TM, TO), TM);
                *nextIterPoints = 3;
                points[1] = T;
                points[2] = M;
            }
            else
            {
                *dir = TO;
                *nextIterPoints = 2;
                points[1] = T;
            }
            
        }
    }
    else // belowTLviaTLM
    {
        if(glm::dot(glm::cross(TLM, TM), TO) < 0) 
        {
            // aboveTMviaTLM between TL and TM according to TLM
            // can only be TLM since -1.0f * TLM didn't get through the plane test
            *dir = TLM;
            *nextIterPoints = 4;
            points[1] = T;
            points[2] = L;
            points[3] = M;

        }
        else // belowTMviaTLM
        {
            if(glm::dot(glm::cross(TM, TMR), TO) >= 0)
            {
                // aboveTMviaTMR
                if(glm::dot(TM, TO) > 0)
                {
                    *dir = TM;
                    *nextIterPoints = 3;
                    points[1] = T;
                    points[2] = M;
                }
                else
                {
                    if(glm::dot(glm::cross(TMR, TR), TO) >= 0)
                    {
                        *dir = TR;
                        *nextIterPoints = 3;
                        points[1] = T;
                        points[2] = R;
                    }
                    else
                    {
                        *dir = T;
                        *nextIterPoints = 2;
                        points[1] = T;
                    }
                }
            }
            else // belowTMviaTMR
            {
                if(glm::dot(glm::cross(TMR, TR), TO) < 0)
                {
                    // aboveTRviaTMR between TM and TR according to TMR
                    // can only be TMR since -1.0f * TMR didn't get through the plane test
                    *dir = TMR;
                    *nextIterPoints = 4;
                    points[1] = T;
                    points[2] = M;
                    points[3] = R;
                }
                else // below TR
                {
                    // can be either TR or T
                    if(glm::dot(TR, TO) >= 0)
                    {
                        *dir = TR;
                        *nextIterPoints = 3;
                        points[1] = T;
                        points[2] = R;
                    }
                    else
                    {
                        *dir = T;
                        *nextIterPoints = 2;
                        points[1] = T;
                    }
                }
            }                        

        }

    }
  
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
            checkLineSegmentSimplex(numPoints, points, dir);
            break;
        }
        case 3:
        {
            checkTriangleSimplex(numPoints, points, dir);
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

            // referencing top, left, middle, and right
            uint32 T, L, M, R;
            
            // test the planes to see if we're behind all of them
            // if behind all, then the tetrahdron contains the origin
            
            glm::vec3 ABC = glm::cross(AB, AC);
            glm::vec3 ACD = glm::cross(AC, AD);
            glm::vec3 ADB = glm::cross(AD, AB);

            int32 planeTest =
                (glm::dot(ABC, AO) >= 0 ? BF_ABC : 0) |
                (glm::dot(ACD, AO) >= 0 ? BF_ACD : 0) |
                (glm::dot(ADB, AO) >= 0 ? BF_ADB : 0);

            switch(planeTest)
            {
                
                case 0:
                {
                    result = true;
                    break;
                }
                case (BF_ABC | BF_ACD | BF_ADB):
                {
                    // how?
                    assert(false);
                    break;
                }
                case BF_ABC | BF_ACD:
                {
                    // check two faces, all 3 edges, and A
                    T = 0; // A
                    L = 1; // B
                    M = 2; // C
                    R = 3; // D
                    checkTwoFacesSimplex(numPoints, points, dir,
                                         T, L, M, R);
                    break;
                }
                case BF_ABC | BF_ADB:
                {
                    // check two faces, all 3 edges, and A
                    T = 0; // A
                    L = 3; // D
                    M = 1; // B
                    R = 2; // C
                    checkTwoFacesSimplex(numPoints, points, dir,
                                         T, L, M, R);
                    break;
                }
                case BF_ACD | BF_ADB:
                {
                    // check two faces, all 3 edges, and A
                    T = 0; // A
                    L = 2; // C
                    M = 3; // D
                    R = 1; // B
                    checkTwoFacesSimplex(numPoints, points, dir,
                                         T, L, M, R);
                    break;
                }
                case BF_ABC:
                {
                    // check face of ABC, 2 edges (AB & AC), and A
                    checkTriangleSimplex(numPoints, points, dir);
                    break;
                }
                case BF_ACD:
                {
                    // check face of ACD, 2 edges (AC & AD), and A
                    points[1] = C;
                    points[2] = D;
                    checkTriangleSimplex(numPoints, points, dir);
                    break;
                }
                case BF_ADB:
                {
                    // check face of ADB, 2 edges (AD & AB), and A
                    points[1] = D;
                    points[2] = B;
                    checkTriangleSimplex(numPoints, points, dir);
                    break;
                }
                
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
