
#ifndef __TreeTest__TreeLeaf__
#define __TreeTest__TreeLeaf__

#include <stdio.h>
#include "Tree.h"
#include "TreeSegment.h"

class TreeLeaf {
public:
    TreeLeaf(Tree *tree, TreeSegment *parent);
    
    void draw();
    void grow(float new_radius);

    const float parent_start_radius;
    float angle_pitch, angle_roll, angle_turn;
    float length_along_parent = 0.5;
    float width = 1, height = 1;
    Tree *tree;
    TreeSegment *parent;
};

#endif
