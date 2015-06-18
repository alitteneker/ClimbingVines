

#ifndef __VineTest__Vine__
#define __VineTest__Vine__

#include <stdio.h>
#include <vector>
#include "ShapeData.h"
#include "GLImage.h"
#include "PlantVoxelMap.h"

class VineSegment;
class VineLeaf;

class Vine {
public:
    Vine(GLuint program, vec4 seed, PlantVoxelMap *voxelmap);
    ~Vine();
    
    void passTime(float time);
    void draw();
    ShapeData* getSegmentShape(float length, float distance);
    ShapeData* getLeafShape();
    float getMaxDistance();
    float getRandom();
    float randLerp(float min, float max);
    float calc_fitness(vec4 point);
    
    vec4 light_location = vec4(10, 30, 10, 1);
    vec4 seed_location;
    PlantVoxelMap *voxelmap;
    
    int num_segments = 0;
    int num_leafs = 0;
    
    // some global metadata, not level specific
    const float max_segment_length    = 0.5f;
    const float min_branch_separation = 1.0f;
    const float branch_probability    = 0.8;
    const float stem_radius           = 0.03f;
    const float growth_rate           = 0.4f;
    const int   num_leaves            = 1;
    
    // material data for drawing
    GLImage *bark_texture, *bark_normal, *bark_gloss;
    GLImage *leaf_texture, *leaf_normal, *leaf_gloss;
    const vec4 colour     = vec4(1, 1, 1, 1);
    const float texalpha  = 1.0f;
    const float bumpiness = 5.0f;
    const float specular  = 0.3f;
    
protected:
    VineSegment* root;
    ShapeData *leafShape;
    vector<ShapeData*> segmentShapes;
    int segmentShapesIndex = 0;
    
    // this controls how many units of segment length correspond to 1 texture unit
    const float segment_UV_length = 5.0f;
};

#endif


