

#include "Vine.h"
#include "VineSegment.h"
#include "VineLeaf.h"
#include "Primitives.h"
#include "ShapeData.h"
#include "MatrixManager.h"
#include "ShaderManager.h"
#include "TextureLoader.h"
#include "matm.h"

float lerp(float scale, float a, float b) {
    return ( ( 1 - scale ) * a ) + ( scale * b );
}
vec4 lerp(float scale, vec4 a, vec4 b) {
    return ( ( 1 - scale ) * a ) + ( scale * b );
}

VineSegment::VineSegment(Vine* vine, VineSegment* parent, float parent_length, bool active, bool dormant) :
max_length(vine->max_segment_length),
length_along_parent(parent_length),
distance  ( ( parent == NULL ) ? 0 : ( parent->distance + parent_length ) ) {
    
    vine->num_segments += 1;
    
    this->vine = vine;
    this->parent = parent;

    is_active = active;
    is_dormant = dormant;
    length = 0;
    dir = normalize(vec4(0,1,0,0));
    if( parent == NULL )
        comp_seg_mat = Translate(vine->seed_location);
    else
        comp_seg_mat = parent->comp_seg_mat * parent->seg_mat * Translate(0,parent->length,0);
}

VineSegment::~VineSegment() {
    while( !stems.empty() ) {
        delete stems[ stems.size() - 1 ];
        stems.pop_back();
    }
    while( !branches.empty() ) {
        delete branches[ branches.size() - 1 ];
        branches.pop_back();
    }
}

bool VineSegment::hasParent() { return parent != NULL; }
bool VineSegment::hasBranch() { return branches.size() > 0; }
bool VineSegment::hasLeaf() { return leaves.size() > 0; }
float VineSegment::getLength() { return length; }

float VineSegment::getMaxDistance() {
    float ret = distance + ( length > 0 ? length : 0 );
    for( int i = 0; i < stems.size(); ++i )
        ret = fmaxf(ret, stems[i]->getMaxDistance());
    for( int i = 0; i < branches.size(); ++i )
        ret = fmaxf(ret, branches[i]->getMaxDistance());
    return ret;
}

float VineSegment::distSinceFeature() {

    if( hasBranch() ) {
        float along = branches[0]->length_along_parent;
        for( int i = 1; i < branches.size(); ++i )
            along = fmaxf(along, branches[i]->length_along_parent);
        return along * length;
    }
    return length + ( hasParent() ? parent->distSinceFeature() : 0 );
}

float VineSegment::distToFeature() {

    if( hasBranch() ) {
        float along = branches[0]->length_along_parent;
        for( int i = 1; i < branches.size(); ++i )
            along = fminf(along, branches[i]->length_along_parent);
        return along * length;
    }
    return length + ( ( stems.size() == 1 ) ? stems[0]->distToFeature() : 0 );
}

void VineSegment::setMaterial() {
    
    set_colour(vine->colour);
    setTextureAlpha(vine->texalpha);
    setBumpScale(vine->bumpiness);
    set_specular(vine->specular);
    
    enableTex(true);   bindTexture(vine->bark_texture, TEX_UNIT);
    enableBump(true);  bindTexture(vine->bark_normal, BUMP_UNIT);
    enableGloss(true); bindTexture(vine->bark_gloss, GLOSS_UNIT);
}

void VineSegment::draw() {
    
    setMaterial();
    
    mstack.push(mModel);
    
    // transform to our own coordinate space
    mModel *= seg_mat;
    
    // TODO: draw a low-res sphere to cover the gap?

    // draw our cylinder
    mstack.push(mModel);
    mModel *= Translate(0, length/2, 0) * Scale(vine->stem_radius, length, vine->stem_radius) * RotateX(-90);
    vine->getSegmentShape(length, distance)->draw();
    mModel = mstack.pop();
    
    // draw leaves and branches
    for( int i = 0; i < leaves.size(); ++i )
        leaves[i]->draw();
    for( int i = 0; i < branches.size(); ++i)
        branches[i]->draw();

    // draw continuing stems or a spherical cap to hide the crimes
    if( stems.size() == 0 ) {
        mstack.push(mModel);
        mModel *= Translate(0, length, 0) * Scale(vine->stem_radius);
        drawSphere();
        mModel = mstack.pop();
    }
    else {
        for( int i = 0; i < stems.size(); ++i)
            stems[i]->draw();
    }
    
    mModel = mstack.pop();
}

void VineSegment::grow(float delt_length) {
    
    float next_delt_length = delt_length;
    if( is_active ) {
        // If we've grown beyond the maximum length
        if( delt_length + length > max_length ) {
            
            next_delt_length = length + delt_length - max_length;
            length = max_length;
            
            // 1) create next stem to continue path
            VineSegment *next = new VineSegment(vine, this, 1, true, false);
            stems.push_back(next);

            // 2) generate leaves
            if( leaves.size() == 0 )
                tryToLeaf();
            
            // 3) probabilistically seed with dormant branches
            tryToBranch();
            
            is_active = false;
        }
        else {
            length += delt_length;

            float best = -INFINITY;
            vec4 new_dir;
            for( int i = 0; i < 16; ++i ) {
                vec4 next = normalize( dir + vec4( vine->randLerp(-0.01, 0.01), 0, vine->randLerp(-0.01, 0.01), 0 ) );
                float test = vine->calc_fitness( comp_seg_mat * (length*next + vec4(0,1)) );
                if( test > best ) {
                    best = test;
                    new_dir = next;
                }
            }
            if( isfinite(best) ) {
                dir = new_dir;
            }
            else
                printf("No valid starting point found.\n");
        }
        
        if( parent == NULL )
            seg_mat.setIdentity();
        else
            seg_mat = Translate(0, parent->getLength() * length_along_parent, 0);
        seg_mat *= RotateVector(dir, vec4(0,1,0,0));
    }
    else {
    }

    // 4) grow all children
    for( int i = 0; i < stems.size(); ++i )
        stems[i]->grow(next_delt_length);
    for( int i = 0; i < branches.size() ; ++i )
        branches[i]->grow(delt_length);
    for( int i = 0; i < leaves.size(); ++i )
        leaves[i]->grow(delt_length);
}

void VineSegment::tryToBranch() {
    bool make_branch = false && distSinceFeature() > vine->min_branch_separation && vine->getRandom() < vine->branch_probability;
    if( make_branch ) {
        const float along = 0.5f;
        VineSegment* branch = new VineSegment(vine, this, along, false, true);
        branches.push_back(branch);
    }
}

void VineSegment::tryToLeaf() {
    for( int i = 0; i < vine->num_leaves; ++i ) {
        VineLeaf* leaf = new VineLeaf(vine, this);
        leaf->angle_pitch = vine->randLerp(20, 45);
        leaf->angle_roll = vine->randLerp(0, 360);
        leaf->length_along_parent = vine->randLerp(0.1, 0.9);
        leaves.push_back(leaf);
    }
}



