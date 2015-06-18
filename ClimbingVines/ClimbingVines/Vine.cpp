

#include "Vine.h"
#include "VineSegment.h"
#include "PlantPrimitives.h"
#include "TextureLoader.h"
#include "ShaderManager.h"
#include "MatrixManager.h"
#include "Primitives.h"
#include "Ray.h"


Vine::Vine(GLuint program, vec4 seed, PlantVoxelMap *voxelmap) {
    // construct the leaf shape
    leafShape = genLeaf(program);
    
    segmentShapes.push_back(genTaperedCylinder(program, 1, 0, 1));
    
    this->voxelmap = voxelmap;
    this->seed_location = seed;
    
    // load all textures
    bark_texture  = loadTexture( "treebark.tga"          );
    bark_gloss    = loadTexture( "treebark_grey.tga"     );
    bark_normal   = loadTexture( "treebark_normal.tga"   );
    leaf_texture  = loadTexture( "leaf_texture.tga"      );
    leaf_gloss    = loadTexture( "leaf_texture_grey.tga" );
    leaf_normal   = loadTexture( "leaf_normal.tga"       );
//    dirt_texture  = loadTexture( "dirt_texture.tga"      );
//    dirt_normal   = loadTexture( "dirt_normal.tga"       );
//    dirt_gloss    = loadTexture( "dirt_gloss.tga"        );
    
    // finally, build our root node
    root = new VineSegment(this, NULL, 0, true, false);
}
Vine::~Vine() {
    delete leafShape;
    delete root;
}

float Vine::getMaxDistance() {
    return root->getMaxDistance();
}

float Vine::getRandom() {
    return rand()/(float)RAND_MAX;
}
float Vine::randLerp(float min, float max) {
    return min + getRandom() * ( max - min );
}

void Vine::passTime(float time_diff) {
    root->grow(time_diff*growth_rate);
}

void Vine::draw() {
    
    // draw the dirt : leaving this around in comments just because it's ind of cool
//    enableTex(true);
//    enableBump(true);
//    enableGloss(true);
//    bindTexture(dirt_texture, TEX_UNIT);
//    bindTexture(dirt_gloss, GLOSS_UNIT);
//    bindTexture(dirt_normal, BUMP_UNIT);
//    set_colour(1,1,1);
//    setTextureAlpha(1.0);
//    setBumpScale(20.0);
//    set_specular(vec4(0.1f));
//    mstack.push(mModel);
//    mModel *= Scale(2.5, 0.25, 2.5);
//    drawCube();
//    mModel = mstack.pop();
    
    // Draw the actual Vine!
    mstack.push(mModel);
    mModel *= Translate(seed_location);
    root->draw();
    mModel = mstack.pop();
}

ShapeData* Vine::getLeafShape() {
    return leafShape;
}
ShapeData* Vine::getSegmentShape(float length, float distance) {
    
//    segmentShapeIndex = ( segmentShapeIndex + 1 ) % segmentShapes.size();
//    ShapeData* ret = segmentShapes[segmentShapeIndex];
//    
//    const float UV_top = fmodf(distance/segment_UV_length, 1.0f);
//    const float UV_bottom = UV_top + (length/segment_UV_length);
//    
//    modifyTaperedCylinder(ret, radius, UV_top, UV_bottom);
//    
//    return ret;
    if( segmentShapesIndex == segmentShapes.size() )
        segmentShapesIndex = 0;
    return segmentShapes[segmentShapesIndex++];
}

float Vine::calc_fitness(vec4 point) {
    if( point.y < 0 || voxelmap->is_occupied(point) )
        return -INFINITY;
    if( voxelmap->has_occupied_voxel( Ray(point, light_location-point), 0, 1 ) )
        return -INFINITY;
    return length(((PlantVoxel*)voxelmap->get_voxel(voxelmap->get_index(point)))->closest_center-point);
}

