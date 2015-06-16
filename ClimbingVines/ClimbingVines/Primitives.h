
#ifndef __TreeTest__Primitives__
#define __TreeTest__Primitives__

#include "ShapeData.h"

void initPrimitives(GLuint program);

ShapeData* genCube(GLuint program, float tex_size);

void drawCube();
void drawSphere();
void drawCone();
void drawCylinder();

void makeCircleWall(vec4* destp, vec3* destn, int numDivisions, float z1, float z2, int& Index, int dir);
void makeCircleWall(vec4* destp, vec3* destn, float radius, int numDivisions, float z1, float z2, int& Index, int dir);
void makeCircle(vec2* dest, int numDivisions, float radius);
void makeCircle(vec2* dest, int numDivisions);

#endif
