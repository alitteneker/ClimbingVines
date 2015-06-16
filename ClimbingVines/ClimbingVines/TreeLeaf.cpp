
#include "TreeLeaf.h"
#include "TreeSegment.h"
#include "Tree.h"
#include "ShapeData.h"
#include "MatrixManager.h"
#include "ShaderManager.h"

TreeLeaf::TreeLeaf(Tree *tree, TreeSegment *parent) :
parent_start_radius( parent->getRadius() ){
    
    tree->num_leafs += 1;
    this->tree = tree;
    this->parent = parent;
    
    angle_turn = tree->randLerp(0, 360);
}

void TreeLeaf::grow(float new_radius) {
    float size = powf( ( new_radius - parent_start_radius ) / tree->max_segment_radius, tree->leaf_size_exp );
    width = size;
    height = size;
}

void TreeLeaf::draw() {
    
    enableTex(true); enableBump(true); enableGloss(true);
    bindTexture(tree->leaf_texture, TEX_UNIT);
    bindTexture(tree->leaf_gloss, GLOSS_UNIT);
    bindTexture(tree->leaf_normal, BUMP_UNIT);
    
    setTextureAlpha(1.0);
    set_colour(1.0, 1.0, 1.0);
    set_specular(0.3f);
    setBumpScale(0.5f);
    
    mstack.push(mModel);
    
    float outer_width = parent->getRadius() - length_along_parent * (parent->getRadius() - parent->getSecondRadius());
    mModel *= Translate(0, parent->getLength() * length_along_parent, 0)
        * RotateY(angle_roll)
        * Translate(-outer_width, 0, 0)
        * RotateZ(angle_pitch)
        * RotateY(angle_turn)
        * Scale(width, height, width * height);
    tree->getLeafShape()->draw();
    
    mModel = mstack.pop();
}


