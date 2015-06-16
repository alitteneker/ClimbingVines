

#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <cmath>
#include "matm.h"
#include "ShapeData.h"
#include "Primitives.h"

//----------------------------------------------------------------------------
// Cube

const int numCubeVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)
ShapeData cubeData = ShapeData(numCubeVertices);

// Vertices of a unit cube centered at origin, sides aligned with axes
vec4 vertices[8] = {
    vec4( -0.5, -0.5,  0.5, 1.0 ),
    vec4( -0.5,  0.5,  0.5, 1.0 ),
    vec4(  0.5,  0.5,  0.5, 1.0 ),
    vec4(  0.5, -0.5,  0.5, 1.0 ),
    vec4( -0.5, -0.5, -0.5, 1.0 ),
    vec4( -0.5,  0.5, -0.5, 1.0 ),
    vec4(  0.5,  0.5, -0.5, 1.0 ),
    vec4(  0.5, -0.5, -0.5, 1.0 )
};

// quad generates two triangles for each face and assigns normals and texture coordinates to the vertices
void quad( ShapeData* data, int &Index, int a, int b, int c, int d, float tex_size, const vec3& normal, const vec3& tangent ) {
    
    data->points[Index] = vertices[a]; data->normals[Index] = normal; data->tangents[Index] = tangent;
    data->UVs[Index] = vec2(0.0f, tex_size); Index++;
    data->points[Index] = vertices[b]; data->normals[Index] = normal; data->tangents[Index] = tangent;
    data->UVs[Index] = vec2(0.0f, 0.0f); Index++;
    data->points[Index] = vertices[c]; data->normals[Index] = normal; data->tangents[Index] = tangent;
    data->UVs[Index] = vec2(tex_size, 0.0f); Index++;
    data->points[Index] = vertices[a]; data->normals[Index] = normal; data->tangents[Index] = tangent;
    data->UVs[Index] = vec2(0.0f, tex_size); Index++;
    data->points[Index] = vertices[c]; data->normals[Index] = normal; data->tangents[Index] = tangent;
    data->UVs[Index] = vec2(tex_size, 0.0f); Index++;
    data->points[Index] = vertices[d]; data->normals[Index] = normal; data->tangents[Index] = tangent;
    data->UVs[Index] = vec2(tex_size, tex_size); Index++;
}

// generate 12 triangles: 36 vertices, 36 normals, 36 texture coordinates
void colorcube(ShapeData* data, float tex_size) {
    int Index = 0;
    quad( data, Index, 1, 0, 3, 2, tex_size, vec3( 0.0f,  0.0f,  1.0f), vec3( 0.0f,  1.0f,  0.0f) );
    quad( data, Index, 2, 3, 7, 6, tex_size, vec3( 1.0f,  0.0f,  0.0f), vec3( 0.0f,  0.0f,  1.0f) );
    quad( data, Index, 3, 0, 4, 7, tex_size, vec3( 0.0f, -1.0f,  0.0f), vec3( 1.0f,  0.0f,  0.0f) );
    quad( data, Index, 6, 5, 1, 2, tex_size, vec3( 0.0f,  1.0f,  0.0f), vec3(-1.0f,  0.0f,  0.0f) );
    quad( data, Index, 4, 5, 6, 7, tex_size, vec3( 0.0f,  0.0f, -1.0f), vec3( 0.0f, -1.0f,  0.0f) );
    quad( data, Index, 5, 4, 0, 1, tex_size, vec3(-1.0f,  0.0f,  0.0f), vec3( 0.0f,  0.0f, -1.0f) );
}

// initialization
void generateCube(GLuint program, ShapeData* data) {
    
    colorcube(data, 1);
    data->enableTangentArray( program, true );
    data->enableUVArray( program, true );
    data->bind(program);
}

ShapeData* genCube(GLuint program, float tex_size) {
    ShapeData* ret = new ShapeData(numCubeVertices);
    colorcube(ret, tex_size);
    ret->enableTangentArray( program, true );
    ret->enableUVArray( program, true );
    ret->bind(program);
    return ret;
}


//----------------------------------------------------------------------------
// Sphere approximation by recursive subdivision of a tetrahedron

const int N = 5;  // number of subdivisions
const int numSphereVertices = 16*256*3;  // number of resulting points

ShapeData sphereData = ShapeData(numSphereVertices);

// four equally spaced points on the unit circle
vec4 v[4] = {
    vec4(0.0, 0.0, 1.0, 1.0),
    vec4(0.0, 0.942809, -0.333333, 1.0),
    vec4(-0.816497, -0.471405, -0.333333, 1.0),
    vec4(0.816497, -0.471405, -0.333333, 1.0)
};

static int k = 0;

// move a point to unit circle

vec4 unit(const vec4 &p) {
    vec4 c;
    double d=0.0;
    for(int i=0; i<3; i++)
        d+=p[i]*p[i];
    d=sqrt(d);
    if(d > 0.0)
        for(int i=0; i<3; i++)
            c[i] = p[i]/d;
    c[3] = 1.0;
    return c;
}

void triangle(vec4 a, vec4 b, vec4 c) {
    sphereData.points[k] = a;
    k++;
    sphereData.points[k] = b;
    k++;
    sphereData.points[k] = c;
    k++;
}

void divide_triangle(vec4 a, vec4 b, vec4 c, int n) {
    vec4 v1, v2, v3;
    if(n>0) {
        v1 = unit(a + b);
        v2 = unit(a + c);
        v3 = unit(b + c);
        divide_triangle(a , v2, v1, n-1);
        divide_triangle(c , v3, v2, n-1);
        divide_triangle(b , v1, v3, n-1);
        divide_triangle(v1, v2, v3, n-1);
    }
    else triangle(a, b, c);
}

void tetrahedron(int n) {
    divide_triangle(v[0], v[1], v[2], n);
    divide_triangle(v[3], v[2], v[1], n);
    divide_triangle(v[0], v[3], v[1], n);
    divide_triangle(v[0], v[3], v[2], n);
}

// initialization

void generateSphere(GLuint program, ShapeData* sphereData)
{
    tetrahedron(N);
    
    // Normals and tangents
    for (int i = 0; i < numSphereVertices; i++) {
        sphereData->normals[i] = vec3(sphereData->points[i].x, sphereData->points[i].y, sphereData->points[i].z);
    }
    
    // TexCoords
    double u, v;
    for (int i = 0; i < numSphereVertices; i++) {
        u = 0.5*(atan2(sphereData->points[i].z, sphereData->points[i].x)/M_PI + 1); //0~1
        v = 1 + asin(sphereData->points[i].y)/M_PI - 0.5; //0~1, and right ways up
        
        // make sure that we have no seam
        if( i % 3 > 0 ) {
            vec2 prev = sphereData->UVs[i-(i%3)];
            if( fabs(u - prev.x) > 0.5 ) {
                float diff = ( u < prev.x ) ? ( u + 1 - prev.x ) : ( u - 1 - prev.x );
                u = prev.x + diff;
            }
        }
        sphereData->UVs[i] = vec2(u,v);
    }
    
    // Create a vertex array object
    sphereData->enableUVArray( true, program );
    sphereData->bind(program);
}

//----------------------------------------------------------------------------
// Cone

const int numConeDivisions = 32;
const int numConeVertices = numConeDivisions * 6;

ShapeData coneData = ShapeData(numConeVertices);

void generateCone(GLuint program, ShapeData* coneData) {
    
    vec2 circlePoints[numConeDivisions];
    makeCircle(circlePoints, numConeDivisions);
    int Index = 0;
    makeCircleWall(coneData->points, coneData->normals, numConeDivisions, 1.0f, 1.0f, Index, 1);
    makeCircleWall(coneData->points, coneData->normals, numConeDivisions, 1.0f, -1.0f, Index, -1);
    
    // Create a vertex array object
    coneData->bind(program);
}

//----------------------------------------------------------------------------
// Cylinder

const int numCylDivisions = 32;
const int numCylVertices = numCylDivisions * 12; // (3*top)+(3*bottom)+(6*side)

ShapeData cylData = ShapeData(numCylVertices);

void generateCylinder(GLuint program, ShapeData* cylData) {
    int Index = 0;
    vec3 tangent = vec3(0.0f, 0.0f, 1.0f);
    vec2 circlePoints[numCylDivisions];
    makeCircle(circlePoints, numCylDivisions);
    makeCircleWall(cylData->points, cylData->normals, numCylDivisions, 1.0f, 1.0f, Index, 1);
    makeCircleWall(cylData->points, cylData->normals, numCylDivisions, -1.0f, -1.0f, Index, -1);
    
    for (int i = 0; i < numCylDivisions; i++)
    {
        int i2 = (i+1)%numCylDivisions;
        float iAngle  = ( i  / (float)numCylDivisions );
        float i2Angle = ( (i+1) / (float)numCylDivisions );
        
        vec3 p1(circlePoints[i2].x, circlePoints[i2].y, -1.0f);
        vec3 p2(circlePoints[i2].x, circlePoints[i2].y,  1.0f);
        vec3 p3(circlePoints[i].x,  circlePoints[i].y,   1.0f);
        cylData->points[Index] = p1; cylData->normals[Index] = vec3(p1.x, p1.y, 0.0f); cylData->tangents[Index] = tangent;
        cylData->UVs[Index] = vec2(i2Angle, 1); Index++;
        cylData->points[Index] = p2; cylData->normals[Index] = vec3(p2.x, p2.y, 0.0f); cylData->tangents[Index] = tangent;
        cylData->UVs[Index] = vec2(i2Angle, 0); Index++;
        cylData->points[Index] = p3; cylData->normals[Index] = vec3(p3.x, p3.y, 0.0f); cylData->tangents[Index] = tangent;
        cylData->UVs[Index] = vec2(iAngle, 0); Index++;
        
        p1 = vec3(circlePoints[i2].x, circlePoints[i2].y, -1.0f);
        p2 = vec3(circlePoints[i].x,  circlePoints[i].y,   1.0f);
        p3 = vec3(circlePoints[i].x,  circlePoints[i].y,  -1.0f);
        cylData->points[Index] = p1; cylData->normals[Index] = vec3(p1.x, p1.y, 0.0f); cylData->tangents[Index] = tangent;
        cylData->UVs[Index] = vec2(i2Angle, 1); Index++;
        cylData->points[Index] = p2; cylData->normals[Index] = vec3(p2.x, p2.y, 0.0f); cylData->tangents[Index] = tangent;
        cylData->UVs[Index] = vec2(iAngle, 0); Index++;
        cylData->points[Index] = p3; cylData->normals[Index] = vec3(p3.x, p3.y, 0.0f); cylData->tangents[Index] = tangent;
        cylData->UVs[Index] = vec2(iAngle, 1); Index++;
    }
    
    // Create a vertex array object
    cylData->enableTangentArray( program, true );
    cylData->enableUVArray( program, true );
    cylData->bind(program);
}

void drawCylinder(void) {
    drawShape(&cylData);
}

void drawCone(void) {
    drawShape(&coneData);
}

void drawCube(void) {
    drawShape(&cubeData);
}

void drawSphere(void) {
    drawShape(&sphereData);
}

void initPrimitives(GLuint program) {
    generateCube    ( program, &cubeData   );
    generateSphere  ( program, &sphereData );
    generateCone    ( program, &coneData   );
    generateCylinder( program, &cylData    );
}

//----------------------------------------------------------------------------
// Generic utilities, used in many places
void makeCircleWall(vec4* destp, vec3* destn, int numDivisions, float z1, float z2, int& Index, int dir) {
    makeCircleWall(destp, destn, 1.0f, numDivisions, z1, z2, Index, dir);
}
void makeCircleWall(vec4* destp, vec3* destn, float radius, const int numDivisions, float z1, float z2, int& Index, int dir) {
    vec2* circlePoints = (vec2*)malloc(sizeof(vec2) * numDivisions);
    makeCircle(circlePoints, numDivisions);
    for (int i = 0; i < numDivisions; i++) {
        vec3 p1(radius*circlePoints[i].x, radius*circlePoints[i].y, z1);
        vec3 p2(0.0f, 0.0f, z2);
        vec3 p3(radius*circlePoints[(i+1)%numDivisions].x, radius*circlePoints[(i+1)%numDivisions].y, z1);
        if (dir == -1) {
            vec3 temp = p1;
            p1 = p3;
            p3 = temp;
        }
        vec3 n = cross(p1-p2, p3-p2);
        destp[Index] = p1; destn[Index] = n; Index++;
        destp[Index] = p2; destn[Index] = n; Index++;
        destp[Index] = p3; destn[Index] = n; Index++;
    }
    delete circlePoints;
}

void makeCircle(vec2* dest, int numDivions) {
    makeCircle(dest, numDivions, 1.0f);
}
void makeCircle(vec2* dest, int numDivisions, float radius) {
    for (int i = 0; i < numDivisions; i++) {
        float a = i * 2.0f * M_PI / numDivisions;
        dest[i] = radius * vec2(cosf(a), sinf(a));
    }
}


