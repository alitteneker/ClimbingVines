
#include "VineLeaf.h"
#include "VineSegment.h"
#include "Vine.h"
#include "ShapeData.h"
#include "MatrixManager.h"
#include "ShaderManager.h"

VineLeaf::VineLeaf(Vine *vine, VineSegment *parent) {
    
    vine->num_leafs += 1;
    this->vine = vine;
    this->parent = parent;
    
    angle_turn = vine->randLerp(0, 360);
}

void VineLeaf::grow(float diff_size) {
    float size = fminf(width + diff_size*0.1, 0.3);
    width = size;
    height = size;
}

void VineLeaf::draw() {
    
    enableTex(true); enableBump(true); enableGloss(true);
    bindTexture(vine->leaf_texture, TEX_UNIT);
    bindTexture(vine->leaf_gloss, GLOSS_UNIT);
    bindTexture(vine->leaf_normal, BUMP_UNIT);
    
    setTextureAlpha(1.0);
    set_colour(1.0, 1.0, 1.0);
    set_specular(vec4(0.3f));
    setBumpScale(0.5f);
    
    mstack.push(mModel);
    
    mModel *= Translate(0, parent->getLength() * length_along_parent, 0)
        * RotateY(angle_roll)
        * Translate(-vine->stem_radius/2, 0, 0)
        * RotateZ(angle_pitch)
        * RotateY(angle_turn)
        * Scale(width, height, width * height);
    vine->getLeafShape()->draw();
    
    mModel = mstack.pop();
}


