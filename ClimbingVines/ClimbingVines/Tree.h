

#ifndef __TreeTest__Tree__
#define __TreeTest__Tree__

#include <stdio.h>
#include <vector>
#include "ShapeData.h"

class TreeSegment;
class TreeLeaf;

struct TreeConfig {
    int num_splits = 2;
    float max_distance = INFINITY;
    float stem_down_angle = 0;
    float min_leaf_distance = INFINITY, min_leaf_separation = INFINITY, leaf_probability = 0;
    float min_leaf_roll = 0, max_leaf_roll = 0;
    float min_branch_distance = INFINITY, min_branch_separation = INFINITY, branch_probability = 0;
    float min_branch_angle = 0, max_branch_angle = 0, min_branch_roll = 0, max_branch_roll = 0;
    int num_leaves;
    float min_split_distance = INFINITY, min_split_separation = 0, split_probability = 0;
    float min_split_angle = 0, max_split_angle = 0;
};

class Tree {
public:
    Tree(GLuint program);
    ~Tree();
    
    float calcSlope(float radius);
    void passTime(float time);
    void draw();
    ShapeData* getSegmentShape(float radius, float length, float distance);
    ShapeData* getWedgeShape(float angle, float distance);
    ShapeData* getLeafShape();
    float getMaxDistance();
    float getRandom();
    float randLerp(float min, float max);
    
    int num_segments = 0;
    int num_leafs = 0;
    const float pot_height = 2.7;
    
    // Tree metadata (level specific segment control the tree needs to know about)
    TreeConfig *genes;
    
    // some global metadata, not level specific
    const int max_levels = 4;
    const float max_segment_length = 0.8f;
    const float min_segment_radius = 0.04f;
    const float max_segment_radius = 0.5f;
    const float min_branch_check   = 0.02f;
    
    // material data for drawing
    GLuint bark_texture, bark_normal, bark_gloss;
    GLuint leaf_texture, leaf_normal, leaf_gloss;
    const float bark_material_exp = 0.1f, leaf_size_exp = 0.3f;
    const vec4 colour_new = vec4(0, 1, 0, 1), colour_old = vec4(1, 1, 1, 1);
    const float texalpha_new = 0.0f,          texalpha_old = 1.0f;
    const float bumpiness_new = 0.0f,         bumpiness_old = 5.0f;
    const float specular_new = 0.4f,          specular_old = 0.3f;
    
protected:
    TreeSegment* root;

    // alpha is slope at 0, beta is the rate of convergence (lower value indicates stronger material)
    const float rad_alpha = 260, rad_beta = 4;

    // time adjustment settings, most are self explanatory
    // shift creates dead space before motion, higher slope creates faster change
    const float time_shift = 0, time_slope = 0.25;
    const float time_zero = logf((max_segment_radius/min_segment_radius)-1)/time_slope;
    
    // this controls how many units of segment length correspond to 1 texture unit
    const float segment_UV_length = 5.0f;
    
    // holders for object streaming
    int segmentShapeIndex = 0, wedgeShapeIndex = 0;
    std::vector<ShapeData*> segmentShapes, wedgeShapes;
    ShapeData* leafShape;
    
    // some additional data for drawing the dirt and pot
    GLuint dirt_texture, dirt_normal, dirt_gloss, clay_texture;
    ShapeData* potShape;
};

#endif


