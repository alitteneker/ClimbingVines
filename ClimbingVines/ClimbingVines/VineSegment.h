
#ifndef __VineTest__VineSegment__
#define __VineTest__VineSegment__

#include <stdio.h>
#include <vector>
#include "ShapeData.h"

class Vine;
class VineLeaf;

class VineSegment {
public:
    VineSegment(Vine* Vine, VineSegment* parent, float along_parent, bool active, bool dormant);
    ~VineSegment();
    
    void draw();
    void setMaterial();
    
    void grow(float time_diff);
    void generateNextStems();
    void tryToBranch();
    void tryToLeaf();
    
    bool  hasParent();
    bool  hasBranch();
    bool  hasLeaf();
    float distSinceFeature();
    float distToFeature();
    mat4 getSegMat();
    
    float getMaxDistance();
    
    float getLength();
    
    const float length_along_parent;
    const float distance, max_length;
    
protected:
    Vine *vine;
    VineSegment *parent;
    std::vector<VineSegment*> stems, branches;
    std::vector<VineLeaf*> leaves;
    
    bool is_active, is_dormant;
    mat4 seg_mat, comp_seg_mat;
    float roll_down = 0;
    float length = 0;
    vec4 dir;
};

#endif
