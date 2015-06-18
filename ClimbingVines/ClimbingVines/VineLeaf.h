
#ifndef __VineTest__VineLeaf__
#define __VineTest__VineLeaf__

#include <stdio.h>
#include "Vine.h"
#include "VineSegment.h"

class VineLeaf {
public:
    VineLeaf(Vine *vine, VineSegment *parent);
    
    void draw();
    void grow(float new_radius);

    float angle_pitch, angle_roll, angle_turn;
    float length_along_parent = 0.5;
    float width = 0, height = 0;
    Vine *vine;
    VineSegment *parent;
};

#endif
