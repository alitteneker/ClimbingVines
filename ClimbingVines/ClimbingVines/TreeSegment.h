
#ifndef __TreeTest__TreeSegment__
#define __TreeTest__TreeSegment__

#include <stdio.h>
#include <vector>
#include "ShapeData.h"

class Tree;
class TreeLeaf;

class TreeSegment {
public:
    TreeSegment(Tree* tree, TreeSegment* parent, float along_parent, bool branch, bool split);
    ~TreeSegment();
    
    void draw();
    void calculateDown();
    void drawWedge(float roll, float pitch);
    void setMaterial();
    
    void grow(float new_radius, float new_slope);
    void grow(float delt_radius);
    void generateNextStems();
    void tryToBranch();
    void tryToLeaf();
    
    bool  hasParent();
    bool  isBranch();
    bool  hasSplit();
    bool  hasBranch();
    bool  hasLeaf();
    float distSinceFeature();
    float distToFeature();
    float distSinceLevel();
    float distToLevel();
    float lastFeatureAngle();
    
    float getMaxDistance();
    
    float getLength();
    float getRadius();
    float getSecondRadius();
    
    const float length_along_parent;
    const bool is_branch, is_split;
    const int level;
    const float distance, max_length;
    const float min_radius;
    
protected:
    Tree* tree;
    TreeSegment* parent;
    std::vector<TreeSegment*> stems, branches;
    std::vector<TreeLeaf*> leaves;
    
    float last_branch_check = 0;
    float roll_down = 0;
    float length = 0, radius_1 = 0, radius_2 = 0, slope = 0;
    float angle_roll=0, angle_pitch=0;
};

#endif
