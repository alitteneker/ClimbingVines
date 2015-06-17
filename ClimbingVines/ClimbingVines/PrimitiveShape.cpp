//
//  PrimitiveShape.cpp
//  ClimbingVines
//
//  Created by Alan Litteneker on 6/16/15.
//  Copyright (c) 2015 Alan Litteneker. All rights reserved.
//

#include "PrimitiveShape.h"
#include "ShaderManager.h"
#include "PTriangle.h"

PrimitiveShape::PrimitiveShape(Material *m) {
    this->material = m;
}

bool PrimitiveShape::addPrimitive(Primitive* prim) {
    if( prim->material != material || shape != NULL )
        return false;
    primitives.push_back(prim);
    return true;
}

void PrimitiveShape::buildShape(GLuint program) {
    shape = new ShapeData( 3 * primitives.size() );
    for( int i = 0; i < primitives.size(); ++i ) {
        Primitive *curr = primitives[i];
        
        PTriangle *triangle = dynamic_cast<PTriangle*>(curr);
        if( triangle ) {
            unsigned long offset = 3*i;
            for( int j = 0; j < 3; ++j ) {
                shape->points[offset+j] = triangle->points[j];
                shape->normals[offset+j] = triangle->normals[j];
                shape->tangents[offset+j] = triangle->tangents[j];
                shape->UVs[offset+j] = triangle->uv_coorinates[j];
            }
        }
        else {
            Error("Unable to build shape for non-triangle primitive.");
        }
    }
    
    shape->enableTangentArray( program, true );
    shape->enableUVArray( program, true );
    shape->bind(program);
}

void PrimitiveShape::draw() {
    writeMaterial(material);
    drawShape(shape);
}
