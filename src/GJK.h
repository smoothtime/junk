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
generalSupport(Mesh *colMesh, glm::vec3 direction)
{
    glm::vec3 result;

    result = colMesh->vertices[0].pos;
    real32 maxDot = glm::dot(direction, result);
    for(uint32 vert = 1; vert < colMesh->numVerts; ++vert)
    {
        real32 testDot = glm::dot(colMesh->vertices[vert].pos, direction);
        if(testDot > maxDot)
        {
            maxDot = testDot;
            result = colMesh->vertices[vert].pos;
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

    real32 dotABAO = glm::dot(AB, AO);
    if(dotABAO > 0)
    {
        points[1] = A;
        points[2] = B;
        *nextIterPoints = 3;
        glm::vec3 tmp = glm::cross(AB, AO);
        *dir = glm::cross(tmp, AB);
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

    glm::vec3 cross = glm::cross(AB, ABC);
    real32 dot = glm::dot(cross, AO);
    if(dot > 0)
    {
        dot = glm::dot(AB, AO);
        if(dot > 0)
        {
            points[1] = A;
            points[2] = B;
            *nextIterPoints = 3;
            glm::vec3 tmp = glm::cross(AB, AO);
            *dir = glm::cross(tmp, AB);
        }
        else
        {
            points[1] = C;
            checkLineSegmentSimplex(nextIterPoints, points, dir);
        }
    }
    else
    {
        cross = glm::cross(ABC, AC);
        dot = glm::dot(cross, AO);
        if(dot > 0)
        {
            points[1] = C;
            checkLineSegmentSimplex(nextIterPoints, points, dir);
        }
        else
        {
            // Inside triangle
            dot = glm::dot(ABC, AO);
            if(dot > 0)
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

    // check the middle edge first to have an even triangle check on either branch path
    if(dot(cross(TLM, TM), TO) < 0)
    {
        if(dot(cross(TL, TLM), TO) > 0)
        {
            // we're on the "left" of TL according to the plane TLM

            if(dot(TL, TO) > 0)
            {
                // it's TL, TLxTOxTL
                points[1] = T;
                points[2] = L;
                *dir = glm::cross(glm::cross(TL, TO), TL);
                *nextIterPoints = 3;
            }
            else
            {
                if(dot(TM, TO) > 0)
                {
                    // it's TM, TMxTOxTM
                    points[1] = T;
                    points[2] = M;
                    *dir = glm::cross(glm::cross(TM, TO), TM);
                    *nextIterPoints = 3;
                }
                else
                {
                    // it's T, TO
                    points[1] = T;
                    *dir = TO;
                    *nextIterPoints = 2;
                }
            }
        }
        else
        {
            // we're in between TL and TM on plane TLM

            // inside triangle
            // can't be negative TLM or else plane test would be different

            // it's TLM, +TLM
            points[1] = T;
            points[2] = L;
            points[3] = M;
            *dir = TLM;
            *nextIterPoints = 4;
        }
    }
    else 
    {
        // we're to the "right" of TM according to the plane TLM

        if(dot(cross(TMR, TR), TO) > 0)
        {
            // we're to the "right" of TR according to the plane TMR

            if(dot(TR, TO) > 0)
            {
                // it's TR, TRxTOxTR
                points[1] = T;
                points[2] = R;
                *dir = glm::cross(glm::cross(TR, TO), TR);
                *nextIterPoints = 3;
            }
            else
            {
                // it's T, TO
                points[1] = T;
                *dir = TO;
                *nextIterPoints = 2;
            }
        }
        else
        {
            // we're in between TM and TR

            if(dot(cross(TM, TMR), TO) > 0)
            {
                // we're to the "left" of M according to the plane TMR

                if(dot(TM, TO) > 0)
                {
                    // it's TM, TMxTOxTM
                    points[1] = T;
                    points[2] = M;
                    *dir = glm::cross(glm::cross(TM, TO), TM);
                    *nextIterPoints = 3;
                }
                else
                {
                    // it's T, TO
                    points[1] = T;
                    *dir = TO;
                    *nextIterPoints = 2;
                }
            }
            else
            {
                // we're in between TM and TR according to plane TMR

                // can only be positive face normal
                // it's TMR, +TMR
                points[1] = T;
                points[2] = M;
                points[3] = R;
                *dir = TMR;
                *nextIterPoints = 4;
            }
        }
    }
}

bool32
doSimplex(int32 *numPoints, glm::vec3 *points, glm::vec3 *dir)
{
    bool32 result = false;
    char log[512];
    sproot(log, "numPoints: %d\n", *numPoints);
    gLog(log);
    real32 GJK_EPISILON = 0.000001;
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


            // TODO(james): delete me later
            real32 ABCdotAO = glm::dot(ABC, AO);
            real32 ACDdotAO = glm::dot(ACD, AO);
            real32 ADBdotAO = glm::dot(ADB, AO);
            
            if((ABCdotAO > 0 && ABCdotAO - GJK_EPISILON < 0) ||
               (ACDdotAO > 0 && ACDdotAO - GJK_EPISILON < 0) ||
               (ADBdotAO > 0 && ADBdotAO - GJK_EPISILON < 0))
            {
                //sprintf_s(log, "ABC %f, ACD %f, ADB %f\n", ABCdotAO, ACDdotAO, ADBdotAO);
                //gLog(log);
                gLog("extremely barely above a face normal");
            }

            int32 planeTest =
                (glm::dot(ABC, AO) > 0 ? BF_ABC : 0) |
                (glm::dot(ACD, AO) > 0 ? BF_ACD : 0) |
                (glm::dot(ADB, AO) > 0 ? BF_ADB : 0);

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
                    // assert(false);
                    result = true;
                    break;
                }
                case BF_ABC | BF_ACD:
                {
                    if(ABCdotAO - GJK_EPISILON < 0 ||
                       ACDdotAO - GJK_EPISILON < 0)
                    {
                        result = true;
                    }
                    else
                    {
                        // check two faces, all 3 edges, and A
                        T = 0; // A
                        L = 1; // B
                        M = 2; // C
                        R = 3; // D
                        checkTwoFacesSimplex(numPoints, points, dir,
                                             T, L, M, R);
                    }
                    break;
                }
                case BF_ABC | BF_ADB:
                {
                    if(ABCdotAO - GJK_EPISILON < 0 ||
                       ADBdotAO - GJK_EPISILON < 0)
                    {
                        result = true;
                    }
                    else
                    {
                        // check two faces, all 3 edges, and A
                        T = 0; // A
                        L = 3; // D
                        M = 1; // B
                        R = 2; // C
                        checkTwoFacesSimplex(numPoints, points, dir,
                                             T, L, M, R);
                    }
                    break;
                }
                case BF_ACD | BF_ADB:
                {
                    if(ACDdotAO - GJK_EPISILON < 0 ||
                       ADBdotAO - GJK_EPISILON < 0)
                    {
                        result = true;
                    }
                    else
                    {
                        // check two faces, all 3 edges, and A
                        T = 0; // A
                        L = 2; // C
                        M = 3; // D
                        R = 1; // B
                        checkTwoFacesSimplex(numPoints, points, dir,
                                             T, L, M, R);
                    }
                    break;
                }
                case BF_ABC:
                {
                    if(ABCdotAO - GJK_EPISILON < 0)
                    {
                        result = true;
                    }
                    else
                    {
                        // check face of ABC, 2 edges (AB & AC), and A
                        checkTriangleSimplex(numPoints, points, dir);
                    }
                    break;
                }
                case BF_ACD:
                {
                    if(ACDdotAO - GJK_EPISILON < 0)
                    {
                        result = true;
                    }
                    else
                    {
                        // check face of ACD, 2 edges (AC & AD), and A
                        points[1] = C;
                        points[2] = D;
                        checkTriangleSimplex(numPoints, points, dir);
                    }
                    break;
                }
                case BF_ADB:
                {
                    if(ADBdotAO - GJK_EPISILON < 0)
                    {
                        result = true;
                    }
                    else
                    {
                        // check face of ADB, 2 edges (AD & AB), and A
                        points[1] = D;
                        points[2] = B;
                        checkTriangleSimplex(numPoints, points, dir);
                    }
                    break;
                }
                
            }
            
            break;
        }
    }
    if(dir->x <= GJK_EPISILON && dir->x >= -1.0f * GJK_EPISILON &&
       dir->y <= GJK_EPISILON && dir->y >= -1.0f * GJK_EPISILON &&
       dir->z <= GJK_EPISILON && dir->z >= -1.0f * GJK_EPISILON)
    {
        // we were right on the simplex
        result = true;
    }
    return result;
}

bool32
gjk(glm::vec3 (*foo)(Mesh*, glm::vec3),
    glm::vec3 (*bar)(Mesh*, glm::vec3),
    Mesh *body1, Mesh *body2)
{
    bool32 x = false;
    glm::vec3 points[4] = {};
    
    glm::vec3 arbitraryDir = glm::vec3(0.0f, 0.0f, -1.0f);//body1->vertices[0].pos;
    glm::vec3 posDir = foo(body1, arbitraryDir);
    glm::vec3 negDir = bar(body2, -1.0f * arbitraryDir);
    
    points[1] = posDir - negDir;
    // search towards origin from here
    glm::vec3 direction = -1.0f * points[1];
    int32 numPoints = 2;

    for(uint32 iterCount = 0;;++iterCount)
    {
        if(iterCount > 1000)
        {
            gLog("what the fuck man");
            // we're super duper close
            x = true;
            break;
        }
        posDir = foo(body1, direction);
        negDir = bar(body2, -1.0f * direction);
        glm::vec3 A = posDir - negDir;
        if(glm::dot(A, direction) < 0)
        {
            // vector from last point to A not going towards origin
            // no intersection
            break;
        }
        else
        {
            points[0] = A;
            if(doSimplex(&numPoints, points, &direction))
            {
                x = true;
                break;
            }
        }
    }
    
    return x;
}

bool32
genericGJK(Mesh *x, Mesh *y)
{
    bool32 test = gjk(&generalSupport, &generalSupport, x, y);
    return test;
}

Mesh
initTestMesh(glm::vec3 basePos, GeneralAllocator *alctr)
{
    Mesh mesh;
    mesh.numVerts = 8;
    mesh.vertices = (Vertex *) alctr->alloc(sizeof(Vertex) * 8);

    mesh.vertices[0].pos = basePos;
    mesh.vertices[1].pos = basePos + glm::vec3( 1.0f,  1.0f, 0.0f);
    mesh.vertices[2].pos = basePos + glm::vec3( 0.0f,  1.0f, 0.0f);
    mesh.vertices[3].pos = basePos + glm::vec3( 1.0f,  0.0f, 0.0f);
    mesh.vertices[4].pos = mesh.vertices[0].pos  + glm::vec3(0.0f, 0.0f, 1.0f);
    mesh.vertices[5].pos = mesh.vertices[1].pos  + glm::vec3(0.0f, 0.0f, 1.0f);
    mesh.vertices[6].pos = mesh.vertices[2].pos  + glm::vec3(0.0f, 0.0f, 1.0f);
    mesh.vertices[7].pos = mesh.vertices[3].pos  + glm::vec3(0.0f, 0.0f, 1.0f);

    for(uint8 i = 0;
        i < 8;
        ++i)
    {
        mesh.vertices[i].pos += glm::vec3(-.0f, 0.0f, 0.0f);
    }

    return mesh;
}

bool32
testGJK(GeneralAllocator *alctr)
{
    Mesh box1 = initTestMesh(glm::vec3(0.0f), alctr);
    Mesh box2 = initTestMesh(glm::vec3(2.0f), alctr);

    return genericGJK(&box1, &box2);

}

#define GJK_H
#endif
