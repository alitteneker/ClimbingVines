

#include "Tree.h"
#include "TreeSegment.h"
#include "PlantPrimitives.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "MatrixManager.h"
#include "Primitives.h"


Tree::Tree(GLuint program) {
    // generate a stock of segment and leaf shapes
    const int STOCK_SHAPES = 8;
    for( int i = 0; i < STOCK_SHAPES; ++i ) {
        segmentShapes.push_back(genTaperedCylinder(program, 1.0f));
        wedgeShapes.push_back(genWedge(program, 45, 1, 0));
    }
    leafShape = genLeaf(program);
    potShape = genPot(program);
    
    // load all textures
    bark_texture  = loadTexture( "treebark.tga"          );
    bark_gloss    = loadTexture( "treebark_grey.tga"     );
    bark_normal   = loadTexture( "treebark_normal.tga"   );
    leaf_texture  = loadTexture( "leaf_texture.tga"      );
    leaf_gloss    = loadTexture( "leaf_texture_grey.tga" );
    leaf_normal   = loadTexture( "leaf_normal.tga"       );
    dirt_texture  = loadTexture( "dirt_texture.tga"      );
    dirt_normal   = loadTexture( "dirt_normal.tga"       );
    dirt_gloss    = loadTexture( "dirt_gloss.tga"        );
    clay_texture  = loadTexture( "clay_texture.tga"      );
    
    // write the genes for this tree
    genes = new TreeConfig[ max_levels + 1 ];

    genes[0].split_probability = 0.2f;
    genes[0].min_split_distance = 3;
    genes[0].num_splits = 3;
    genes[0].min_split_angle = 40;
    genes[0].max_split_angle = 60;
    
    genes[1].max_distance = 10;
    genes[1].stem_down_angle = -15;
    genes[1].split_probability = 0.4f;
    genes[1].min_split_distance = 2;
    genes[1].num_splits = 2;
    genes[1].min_split_angle = 30;
    genes[1].max_split_angle = 40;
    
    genes[2].max_distance = 8;
    genes[2].stem_down_angle = -10;
    genes[2].min_branch_distance = 0.4f;
    genes[2].min_branch_separation = 2;
    genes[2].branch_probability = 0.8f;
    genes[2].min_branch_angle = 20;
    genes[2].max_branch_angle = 35;
    genes[2].max_branch_roll = 45;
    
    genes[3].max_distance = 5;
    genes[3].stem_down_angle = -8;
    genes[3].min_branch_distance = 0.4f;
    genes[3].min_branch_separation = 1;
    genes[3].branch_probability = 0.8f;
    genes[3].min_branch_angle = 30;
    genes[3].max_branch_angle = 45;
    genes[3].max_branch_roll = 60;
    
    genes[4].num_leaves = 3;
    genes[4].stem_down_angle = -7;
    genes[4].min_leaf_distance = 0.5;
    genes[4].leaf_probability = 1;
    genes[4].min_leaf_separation = 0.5f;
    genes[4].max_leaf_roll = 180;
    
    // finally, build our root node
    root = new TreeSegment(this, NULL, 0, false, true);
}
Tree::~Tree() {
    while( !segmentShapes.empty() ) {
        delete segmentShapes[ segmentShapes.size() - 1 ];
        segmentShapes.pop_back();
    }
    while( !wedgeShapes.empty() ) {
        delete wedgeShapes[ wedgeShapes.size() - 1 ];
        wedgeShapes.pop_back();
    }
    delete leafShape;
    delete root;
    delete[]genes;
}

float Tree::getMaxDistance() {
    return root->getMaxDistance();
}

float Tree::getRandom() {
    return rand()/(float)RAND_MAX;
}
float Tree::randLerp(float min, float max) {
    return min + getRandom() * ( max - min );
}
float Tree::calcSlope(float radius) {
    return rad_alpha * logf((rad_beta * radius) + 1) / rad_beta;
}

void Tree::passTime(float new_time) {
    float new_radius = max_segment_radius / ( 1 + expf( -time_slope * ( new_time - (time_zero + time_shift) ) ) );
    root->grow(new_radius, calcSlope(new_radius));
}

void Tree::draw() {

    mModel *= Translate(0, pot_height, 0);
    
    // draw the dirt
    enableTex(true);
    enableBump(true);
    enableGloss(true);
    bindTexture(dirt_texture, TEX_UNIT);
    bindTexture(dirt_gloss, GLOSS_UNIT);
    bindTexture(dirt_normal, BUMP_UNIT);
    set_colour(1,1,1);
    setTextureAlpha(1.0);
    setBumpScale(20.0);
    set_specular(0.1);
    mstack.push(mModel);
    mModel *= Scale(2.5, 0.25, 2.5);
    drawCube();
    mModel = mstack.pop();
    
    // draw the pot
    enableBump(false);
    enableGloss(false);
    bindTexture(clay_texture, TEX_UNIT);
    setTextureAlpha(1.0);
    set_specular(0.05);
    mstack.push(mModel);
    mModel *= Translate(0,-1.2,0) * Scale(3, 3, 3);
    potShape->draw();
    mModel = mstack.pop();
    
    // Draw the actual tree!
    root->draw();
}

ShapeData* Tree::getLeafShape() {
    return leafShape;
}
ShapeData* Tree::getSegmentShape(float radius, float length, float distance) {
    
    segmentShapeIndex = ( segmentShapeIndex + 1 ) % segmentShapes.size();
    ShapeData* ret = segmentShapes[segmentShapeIndex];
    
    const float UV_top = fmodf(distance/segment_UV_length, 1.0f);
    const float UV_bottom = UV_top + (length/segment_UV_length);
    
    modifyTaperedCylinder(ret, radius, UV_top, UV_bottom);
    
    return ret;
}

ShapeData* Tree::getWedgeShape(float angle, float distance) {
    
    wedgeShapeIndex = ( wedgeShapeIndex + 1 ) % wedgeShapes.size();
    ShapeData* ret = wedgeShapes[wedgeShapeIndex];
    
    const float UV_bottom = fmodf(distance/segment_UV_length, 1.0f);
    const float UV_top = UV_bottom - 1/segment_UV_length;
    
    modifyWedge(ret, angle, UV_top, UV_bottom);
    
    return ret;
}


