//  PrimitiveShape.h
//  ClimbingVines

#ifndef __ClimbingVines__PrimitiveShape__
#define __ClimbingVines__PrimitiveShape__

#include <stdio.h>
#include "Primitive.h"
#include "ShapeData.h"
#include <vector>
using namespace std;

// This is a useful handler for combininb all the primitives which share the same
class PrimitiveShape {
public:

    vector<Primitive*> primitives;
    ShapeData *shape = NULL;
    Material *material = NULL;

    PrimitiveShape(Material *m);
    bool addPrimitive(Primitive* prim);
    void buildShape(GLuint program);
    void draw();

};

#endif
