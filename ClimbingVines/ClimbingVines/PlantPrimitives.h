#ifndef __TreeTest__TreePrimitives__
#define __TreeTest__TreePrimitives__

#include <stdio.h>
#include "ShapeData.h"

ShapeData* genTaperedCylinder(GLuint program, float r2);
ShapeData* genTaperedCylinder(GLuint program, const float r2, const float UV_top, const float UV_bottom);
void modifyTaperedCylinder(ShapeData* shape, float radius, float UV_top, float UV_bottom);

ShapeData* genWedge(GLuint program, float angle, float UV_top, float UV_bottom);
void modifyWedge(ShapeData* shape, float angle, float UV_top, float UV_bottom);

ShapeData* genLeaf(GLuint program);
ShapeData* genPot(GLuint program);

#endif
