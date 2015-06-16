

#include "PlantPrimitives.h"
#include "Primitives.h"
#include "matm.h"

const int numCylDivisions = 8;
const int numCylVertices = numCylDivisions * 12; // (3*top)+(3*bottom)+(6*side)
vec2 circlePoints[numCylDivisions];

ShapeData* genTaperedCylinder(GLuint program, const float r2) {
    return genTaperedCylinder(program, r2, 1, 0);
}
ShapeData* genTaperedCylinder(GLuint program, const float r2, const float UV_top, const float UV_bottom) {
    const float r1 = 1.0f;
    ShapeData* cylData = new ShapeData(numCylVertices);
    
    int Index = 0;
    makeCircle(circlePoints, numCylDivisions);
    makeCircleWall(cylData->points, cylData->normals, r2, numCylDivisions, 0.5f,   0.5f, Index,  1);
    makeCircleWall(cylData->points, cylData->normals, r1, numCylDivisions, -0.5f, -0.5f, Index, -1);
    
// Really ought to be using better tans and norms (like in comments below), but that would require that
// we use proper matrix inversions in shaders (fix non-rigid body transforms), and modify the tan/norm
// data for each vertex on modification (below), which is alot of extra work for both me and my computer :P
//    const float denom = 1 + ( (r1-r2) * (r1-r2) );
//    const float l = r1 / denom;
//    const float h = ( r1 * ( r1 - r2 ) ) / denom;
    
    vec3 p11, p12, p13, p21, p22, p23, tan1, tan2, norm1, norm2;
    
    for (int i = 0; i < numCylDivisions; i++) {
    
        int   i2 = (i+1)%numCylDivisions;
        float iAngle  = ( i  / (float)numCylDivisions );
        float i2Angle = ( (i+1) / (float)numCylDivisions );
        
        p11 = vec3( r1 * circlePoints[i2], -0.5f);
        p12 = vec3( r2 * circlePoints[i2],  0.5f);
        p13 = vec3( r2 * circlePoints[i],   0.5f);
        p21 = vec3( r1 * circlePoints[i2], -0.5f);
        p22 = vec3( r2 * circlePoints[i],   0.5f);
        p23 = vec3( r1 * circlePoints[i],  -0.5f);
        
        tan1  = vec3(0,0,1);                // normalize( p13 - p23 );
        tan2  = vec3(0,0,1);                // normalize( p12 - p21 );
        norm1 = vec3(circlePoints[i], 0);   // normalize( vec3( l * circlePoints[i],  h ) );
        norm2 = vec3(circlePoints[i2], 0);  // normalize( vec3( l * circlePoints[i2], h ) );

        cylData->points[Index] = p11;
        cylData->normals[Index] = norm2;
        cylData->tangents[Index] = tan2;
        cylData->UVs[Index] = vec2(i2Angle, UV_top);
        Index++;
        
        cylData->points[Index] = p12;
        cylData->normals[Index] = norm2;
        cylData->tangents[Index] = tan2;
        cylData->UVs[Index] = vec2(i2Angle, UV_bottom);
        Index++;
        
        cylData->points[Index] = p13;
        cylData->normals[Index] = norm1;
        cylData->tangents[Index] = tan1;
        cylData->UVs[Index] = vec2(iAngle, UV_bottom);
        Index++;
        
        cylData->points[Index] = p21;
        cylData->normals[Index] = norm2;
        cylData->tangents[Index] = tan2;
        cylData->UVs[Index] = vec2(i2Angle, UV_top);
        Index++;
        
        cylData->points[Index] = p22;
        cylData->normals[Index] = norm1;
        cylData->tangents[Index] = tan1;
        cylData->UVs[Index] = vec2(iAngle, UV_bottom);
        Index++;
        
        cylData->points[Index] = p23;
        cylData->normals[Index] = norm1;
        cylData->tangents[Index] = tan1;
        cylData->UVs[Index] = vec2(iAngle, UV_top);
        Index++;
    }
    
    // Create a vertex array object
    cylData->enableTangentArray( program, true );
    cylData->enableUVArray( program, true );
    cylData->bind(program);
    
    return cylData;
}

void modifyTaperedCylinder(ShapeData* cylData, float r2, float UV_top, float UV_bottom) {
    
    int Index = 0;
    makeCircleWall(cylData->points, cylData->normals, r2, numCylDivisions, 0.5f, 0.5f, Index, 1);
    Index += numCylDivisions * 3; // no need to change bottom side vertices, radius is unchanged
    
    vec3 p1, p2;
    
    for (int i = 0; i < numCylDivisions; i++) {
        
        int i2 = (i+1)%numCylDivisions;
        float iAngle  = ( i  / (float)numCylDivisions );
        float i2Angle = ( (i+1) / (float)numCylDivisions );
        
        p1 = vec3(r2*circlePoints[i2].x, r2*circlePoints[i2].y,  0.5f);
        p2 = vec3(r2*circlePoints[i].x,  r2*circlePoints[i].y,   0.5f);

                                      cylData->UVs[Index] = vec2(i2Angle, UV_top    );  ++Index;
        cylData->points[Index] = p1;  cylData->UVs[Index] = vec2(i2Angle, UV_bottom );  ++Index;
        cylData->points[Index] = p2;  cylData->UVs[Index] = vec2(iAngle,  UV_bottom );  ++Index;
                                      cylData->UVs[Index] = vec2(i2Angle, UV_top    );  ++Index;
        cylData->points[Index] = p2;  cylData->UVs[Index] = vec2(iAngle,  UV_bottom );  ++Index;
                                      cylData->UVs[Index] = vec2(iAngle,  UV_top    );  ++Index;
    }
    
    // sub in the new data
    cylData->subPoints();
    cylData->subUVs();
}

void modifyCircleWall(vec4* destp, vec3* destn, float radius, int numDivisions, float z1, float z2, int& Index, int dir) {
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
}

const int numWedgeDivisions = numCylDivisions / 2;
const int numWedgeVertices = 6 * numWedgeDivisions;

ShapeData* genWedge(GLuint program, float angle, float UV_top, float UV_bottom) {
    ShapeData* ret = new ShapeData(numWedgeVertices);
    makeCircle(circlePoints, numCylDivisions);
    
    int Index = 0;
    mat4 rot = RotateX(angle);
    vec3 norm1, norm2, tan1, tan2;
    vec4 p11, p12, p13, p21, p22, p23;
    for( int i = 0; i < numWedgeDivisions; ++i ) {
        int i2 = (i+1)%numCylDivisions;
        float iAngle  = ( i  / (float)numCylDivisions );
        float i2Angle = ( (i+1) / (float)numCylDivisions );
        
        tan1  = vec3(0,0,1);
        tan2  = vec3(0,0,1);
        norm1 = vec3(circlePoints[i], 0);
        norm2 = vec3(circlePoints[i2], 0);
        
        p11 = vec4( circlePoints[i2], 0.0f, 1);
        p12 = vec4( circlePoints[i2], 0.0f, 1) * rot; // top
        p13 = vec4( circlePoints[i],  0.0f, 1) * rot; // top
        p21 = vec4( circlePoints[i2], 0.0f, 1);
        p22 = vec4( circlePoints[i],  0.0f, 1) * rot; // top
        p23 = vec4( circlePoints[i],  0.0f, 1);
        
        ret->points[Index] = p11;
        ret->normals[Index] = norm2;
        ret->tangents[Index] = tan2;
        ret->UVs[Index] = vec2(i2Angle, UV_top);
        Index++;
        
        ret->points[Index] = p12;
        ret->normals[Index] = norm2;
        ret->tangents[Index] = tan2;
        ret->UVs[Index] = vec2(i2Angle, UV_bottom);
        Index++;
        
        ret->points[Index] = p13;
        ret->normals[Index] = norm1;
        ret->tangents[Index] = tan1;
        ret->UVs[Index] = vec2(iAngle, UV_bottom);
        Index++;
        
        ret->points[Index] = p21;
        ret->normals[Index] = norm2;
        ret->tangents[Index] = tan2;
        ret->UVs[Index] = vec2(i2Angle, UV_top);
        Index++;
        
        ret->points[Index] = p22;
        ret->normals[Index] = norm1;
        ret->tangents[Index] = tan1;
        ret->UVs[Index] = vec2(iAngle, UV_bottom);
        Index++;
        
        ret->points[Index] = p23;
        ret->normals[Index] = norm1;
        ret->tangents[Index] = tan1;
        ret->UVs[Index] = vec2(iAngle, UV_top);
        Index++;
    }
    
    // Create the vertex array object
    ret->enableTangentArray( program, true );
    ret->enableUVArray( program, true );
    ret->bind(program);
    
    return ret;
}

void modifyWedge(ShapeData* wedgeData, float angle, float UV_top, float UV_bottom) {
    int Index = 0;
    
    vec4 p1, p2;
    mat4 rot = RotateX(angle);
    for (int i = 0; i < numWedgeDivisions; i++) {
        
        int i2 = (i+1)%numCylDivisions;
        float iAngle  = -0.25 + ( i  / (float)numCylDivisions );
        float i2Angle = -0.25 + ( (i+1) / (float)numCylDivisions );
        
        p1 = vec4(circlePoints[i2].x, circlePoints[i2].y,  0.0f, 1) * rot;
        p2 = vec4(circlePoints[i].x,  circlePoints[i].y,   0.0f, 1) * rot;
        
                                        wedgeData->UVs[Index] = vec2(i2Angle, UV_top);  ++Index;
        wedgeData->points[Index] = p1;  wedgeData->UVs[Index] = vec2(i2Angle, UV_top);  ++Index;
        wedgeData->points[Index] = p2;  wedgeData->UVs[Index] = vec2(iAngle,  UV_top);  ++Index;
                                        wedgeData->UVs[Index] = vec2(i2Angle, UV_top);  ++Index;
        wedgeData->points[Index] = p2;  wedgeData->UVs[Index] = vec2(iAngle,  UV_top);  ++Index;
                                        wedgeData->UVs[Index] = vec2(iAngle,  UV_top);  ++Index;
    }
    
    // sub in the new data
    wedgeData->subPoints();
    wedgeData->subUVs();
}

void writeTriangle(int &Index, ShapeData* ret, int index1, int index2, int index3, vec4 *points, vec4 *norms, vec4 *tangs, vec4 offset, vec2 tex_off, vec2 tex_scale, mat4 mat1, mat4 mat2) {

    ret->points[Index]   = offset + ( mat2 * points[index1] );
    ret->normals[Index]  = mat1 * norms[index1];
    ret->tangents[Index] = mat1 * tangs[index1];
    ret->UVs[Index]      = tex_scale * ( tex_off + vec2(mat1 * points[index1]) );
    ++Index;
    ret->points[Index]   = offset + ( mat2 * points[index2] );
    ret->normals[Index]  = mat1 * norms[index2];
    ret->tangents[Index] = mat1 * tangs[index2];
    ret->UVs[Index]      = tex_scale * ( tex_off + vec2(mat1 * points[index2]) );
    ++Index;
    ret->points[Index]   = offset + ( mat2 * points[index3] );
    ret->normals[Index]  = mat1 * norms[index3];
    ret->tangents[Index] = mat1 * tangs[index3];
    ret->UVs[Index]      = tex_scale * ( tex_off + vec2(mat1 * points[index3]) );
    ++Index;
}

void writeQuad(int &Index, ShapeData* ret, vec4 a, vec4 b, vec4 c, vec4 d, vec3 norm, vec3 tang) {
    ret->points[Index] = a; ret->normals[Index] = norm; ret->tangents[Index] = tang; ret->UVs[Index] = vec2(0,0); ++Index;
    ret->points[Index] = b; ret->normals[Index] = norm; ret->tangents[Index] = tang; ret->UVs[Index] = vec2(0,0); ++Index;
    ret->points[Index] = c; ret->normals[Index] = norm; ret->tangents[Index] = tang; ret->UVs[Index] = vec2(0,0); ++Index;

    ret->points[Index] = a; ret->normals[Index] = norm; ret->tangents[Index] = tang; ret->UVs[Index] = vec2(0,0); ++Index;
    ret->points[Index] = d; ret->normals[Index] = norm; ret->tangents[Index] = tang; ret->UVs[Index] = vec2(0,0); ++Index;
    ret->points[Index] = c; ret->normals[Index] = norm; ret->tangents[Index] = tang; ret->UVs[Index] = vec2(0,0); ++Index;
}

void writePyramid(int &Index, ShapeData* ret, vec4 bottom, vec4 top, float wid, float leng) {
    vec4 points[5];
    vec4 wide1( wid/2, 0, 0, 0);
    vec4 wide2(0, 0, leng/2, 0);
    points[0] = top;
    points[1] = bottom + wide1;
    points[2] = bottom - wide2;
    points[3] = bottom - wide1;
    points[4] = bottom + wide2;
    
    for( int i = 1; i < 5; ++i ) {
        int i2 = ( i + 1 ) % 5;
        if( i2 == 0 )
            i2 = 1;
        
        ret->points[Index]   = points[0];
        ret->normals[Index]  = normalize(cross(points[i]-points[0], points[i2]-points[0]));
        ret->tangents[Index] = vec3(0, 0, 0);
        ret->UVs[Index]      = vec2(0.5, 0);
        ++Index;
        ret->points[Index]   = points[i];
        ret->normals[Index]  = normalize(points[i]);
        ret->tangents[Index] = vec3(0, 0, 0);
        ret->UVs[Index]      = vec2(0.5, 0);
        ++Index;
        ret->points[Index]   = points[i2];
        ret->normals[Index]  = normalize(points[i2]);
        ret->tangents[Index] = vec3(0, 0, 0);
        ret->UVs[Index]      = vec2(0.5, 0);
        ++Index;
    }
}

const int num_leaf_points = 14;
const int num_leaf_polys = 4 + 4*(num_leaf_points-2);
const int num_leaf_vertices = 3 * num_leaf_polys;
ShapeData* genLeaf(GLuint program) {
    ShapeData* ret = new ShapeData(num_leaf_vertices);
    int Index = 0;
    
    vec4 offset(0, 0.5, 0.01, 0);
    vec2 tex_off(0.5, 0.1);
    vec2 tex_scale(1.0, 1/1.1);
    
    vec4 points[num_leaf_points];
    vec4 norms[num_leaf_points];
    vec4 tangs[num_leaf_points];
    points[0]  = vec4(0,      0.035, 0.02, 1);  norms[0]  = normalize(vec4( 0,0,1,0));  tangs[0]  = vec4(0,1,0,0);
    points[1]  = vec4(0.02,   0.035, 0.02, 1);  norms[1]  = normalize(vec4( 0,0,1,0));  tangs[1]  = vec4(0,1,0,0);
    points[2]  = vec4(0.13,  -0.063, 0.03, 1);  norms[2]  = normalize(vec4(-1,0,1,0));  tangs[2]  = vec4(0,1,0,0);
    points[3]  = vec4(0.18,  -0.09,  0.05, 1);  norms[3]  = normalize(vec4(-1,0,1,0));  tangs[3]  = vec4(0,1,0,0);
    points[4]  = vec4(0.25,  -0.1,   0.1,  1);  norms[4]  = normalize(vec4(-1,0,1,0));  tangs[4]  = vec4(0,1,0,0);
    points[5]  = vec4(0.375, -0.05,  0.15, 1);  norms[5]  = normalize(vec4(-1,0,1,0));  tangs[5]  = vec4(0,1,0,0);
    points[6]  = vec4(0.465,  0.08,  0.2,  1);  norms[6]  = normalize(vec4(-1,0,1,0));  tangs[6]  = vec4(0,1,0,0);
    points[7]  = vec4(0.49,   0.2,   0.25, 1);  norms[7]  = normalize(vec4(-1,0,1,0));  tangs[7]  = vec4(0,1,0,0);
    points[8]  = vec4(0.49,   0.3,   0.25, 1);  norms[8]  = normalize(vec4(-1,0,1,0));  tangs[8]  = vec4(0,1,0,0);
    points[9]  = vec4(0.475,  0.4,   0.26, 1);  norms[9]  = normalize(vec4(-1,0,1,0));  tangs[9]  = vec4(0,1,0,0);
    points[10] = vec4(0.415,  0.55,  0.23, 1);  norms[10] = normalize(vec4(-1,0,1,0));  tangs[10] = vec4(0,1,0,0);
    points[11] = vec4(0.31,   0.7,   0.18, 1);  norms[11] = normalize(vec4(-1,0,1,0));  tangs[11] = vec4(0,1,0,0);
    points[12] = vec4(0.165,  0.85,  0.13, 1);  norms[12] = normalize(vec4(-1,0,1,0));  tangs[12] = vec4(0,1,0,0);
    points[13] = vec4(0,      1,     0.05, 1);  norms[13] = normalize(vec4( 0,0,1,0));  tangs[13] = vec4(0,1,0,0);
    
    mat4 flipX = Scale( -1, 1,  1 );
    mat4 flipZ = Scale( 1, 1,  -1 );
    mat4 mOff = Translate(0, 0, -0.001);
    mat4 ident = mat4();
    
    // build leaf stem (connects to origin)
    const float stem_width = 0.025;
    vec4 stem_bottom = vec4(0,0,0,1), stem_top = offset + points[num_leaf_points-1];
    writePyramid(Index, ret, stem_bottom, stem_top, stem_width, stem_width);
    
    // center ridge, not part of normal fan
    writeTriangle(Index, ret, 0, 1, num_leaf_points-1, points, norms, tangs, offset, tex_off, tex_scale, ident, ident);
    writeTriangle(Index, ret, 0, 1, num_leaf_points-1, points, norms, tangs, offset, tex_off, tex_scale, flipX, flipX);
    
    writeTriangle(Index, ret, 0, 1, num_leaf_points-1, points, norms, tangs, offset, tex_off, tex_scale, flipZ, mOff*ident);
    writeTriangle(Index, ret, 0, 1, num_leaf_points-1, points, norms, tangs, offset, tex_off, tex_scale, flipX*flipZ, mOff*flipX);
    
    // fan from non-central edge
    for( int i = 2; i < num_leaf_points-1; ++i) {
        writeTriangle(Index, ret, 1, i, i+1, points, norms, tangs, offset, tex_off, tex_scale, ident, ident);
        writeTriangle(Index, ret, 1, i, i+1, points, norms, tangs, offset, tex_off, tex_scale, flipX, flipX);
        
        writeTriangle(Index, ret, 1, i, i+1, points, norms, tangs, offset, tex_off, tex_scale, flipZ, mOff*ident);
        writeTriangle(Index, ret, 1, i, i+1, points, norms, tangs, offset, tex_off, tex_scale, flipX*flipZ, mOff*flipX);
    }
    
    // Create the vertex array object
    ret->enableTangentArray( program, true );
    ret->enableUVArray( program, true );
    ret->bind(program);
    
    return ret;
}

void writeQuad(ShapeData* ret, int &Index, vec4 a, vec4 b, vec4 c, vec4 d, vec4 normal, vec4 tangent, mat4 trans) {
    vec4 norm = trans * normal;
    vec3 tang = trans * tangent;
    ret->points[Index] = trans * a; ret->normals[Index] = norm; ret->tangents[Index] = tang;
    ret->UVs[Index] = vec2(0.0f, 1.0f); Index++;
    ret->points[Index] = trans * b; ret->normals[Index] = norm; ret->tangents[Index] = tang;
    ret->UVs[Index] = vec2(0.0f, 0.0f); Index++;
    ret->points[Index] = trans * c; ret->normals[Index] = norm; ret->tangents[Index] = tang;
    ret->UVs[Index] = vec2(1.0f, 0.0f); Index++;
    ret->points[Index] = trans * a; ret->normals[Index] = norm; ret->tangents[Index] = tang;
    ret->UVs[Index] = vec2(0.0f, 1.0f); Index++;
    ret->points[Index] = trans * c; ret->normals[Index] = norm; ret->tangents[Index] = tang;
    ret->UVs[Index] = vec2(1.0f, 0.0f); Index++;
    ret->points[Index] = trans * d; ret->normals[Index] = norm; ret->tangents[Index] = tang;
    ret->UVs[Index] = vec2(1.0f, 1.0f); Index++;
}

ShapeData* genPot(GLuint program) {
    const int numPotVertices = 3*6*4; // 3 quads per side, 6 vertices per quad, 4 sides
    ShapeData* ret = new ShapeData(numPotVertices);
    
    const float inside = 0.1f;
    int Index = 0;
    mat4 rotate = mat4(1.0);
    for( int i = 0; i < 4; ++i ) {
        
        writeQuad(ret, Index,
                  vec4( -0.5+inside, -0.5, 0.5-inside, 1 ),
                  vec4(  0.5-inside, -0.5, 0.5-inside, 1 ),
                  vec4(  0.5,         0.5, 0.5,        1 ),
                  vec4( -0.5,         0.5, 0.5,        1 ),
                  vec4(0,0,1,0), vec4(0,1,0,0), rotate);
        writeQuad(ret, Index,
                  vec4(  0.5,        0.5, 0.5,        1 ),
                  vec4(  0.5-inside, 0.5, 0.5-inside, 1 ),
                  vec4( -0.5+inside, 0.5, 0.5-inside, 1 ),
                  vec4( -0.5,        0.5, 0.5,        1 ),
                  vec4(0,1,0,0), vec4(0,0,1,0), rotate);
        writeQuad(ret, Index,
                  vec4(  0.5-inside, 0.5,        0.5-inside, 1 ),
                  vec4(  0.5-inside, 0.5-inside, 0.5-inside, 1 ),
                  vec4( -0.5+inside, 0.5-inside, 0.5-inside, 1 ),
                  vec4( -0.5+inside, 0.5,        0.5-inside, 1 ),
                  vec4(0,0,-1,0), vec4(0,-1,0,0), rotate);
        
        rotate *= RotateY(90);
    }
    
    // Create the vertex array object
    ret->enableTangentArray( program, true );
    ret->enableUVArray( program, true );
    ret->bind(program);
    
    return ret;
}


