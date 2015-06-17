

#include "Tree.h"
#include "TreeSegment.h"
#include "TreeLeaf.h"
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

TreeSegment::TreeSegment(Tree* tree, TreeSegment* parent, float parent_length, bool branch, bool split) :
max_length(tree->max_segment_length),
length_along_parent(parent_length),
distance  ( ( parent == NULL ) ? 0 : ( parent->distance + parent_length ) ),
min_radius( tree->min_segment_radius ),
level( ( parent == NULL ) ? 0 : ( parent->level + ( ( branch || split ) ? 1 : 0) ) ),
is_branch ( branch ),
is_split  ( split ) {
    
    tree->num_segments += 1;
    
    this->tree = tree;
    this->parent = parent;
    
    if( hasParent() )
        radius_1 = min_radius;

    angle_pitch = tree->genes[level].stem_down_angle / (distSinceLevel() + 1);
}

TreeSegment::~TreeSegment() {
    while( !stems.empty() ) {
        delete stems[ stems.size() - 1 ];
        stems.pop_back();
    }
    while( !branches.empty() ) {
        delete branches[ branches.size() - 1 ];
        branches.pop_back();
    }
}

bool TreeSegment::hasParent() { return parent != NULL; }
bool TreeSegment::isBranch() { return is_branch; }
bool TreeSegment::hasSplit() { return stems.size() > 1; }
bool TreeSegment::hasBranch() { return branches.size() > 0; }
bool TreeSegment::hasLeaf() { return leaves.size() > 0; }
float TreeSegment::getLength() { return length; }
float TreeSegment::getRadius() { return radius_1; }
float TreeSegment::getSecondRadius() { return radius_2; }

float TreeSegment::getMaxDistance() {
    float ret = distance + ( length > 0 ? length : 0 );
    for( int i = 0; i < stems.size(); ++i )
        ret = fmaxf(ret, stems[i]->getMaxDistance());
    for( int i = 0; i < branches.size(); ++i )
        ret = fmaxf(ret, branches[i]->getMaxDistance());
    return ret;
}

float TreeSegment::distSinceLevel() {
    if( is_branch || is_split )
        return length;
    return length + ( hasParent() ? parent->distSinceLevel() : 0 );
}

float TreeSegment::distToLevel() {
    return length + ( ( stems.size() == 1 ) ? stems[0]->distToLevel() : length );
}

float TreeSegment::distSinceFeature() {

    if( hasBranch() ) {
        float along = branches[0]->length_along_parent;
        for( int i = 1; i < branches.size(); ++i )
            along = fmaxf(along, branches[i]->length_along_parent);
        return along * length;
    }
    if( hasLeaf() ) {
        float along = leaves[0]->length_along_parent;
        for( int i = 1; i < leaves.size(); ++i )
            along = fmaxf(along, leaves[i]->length_along_parent);
        return along * length;
    }
    return length + ( hasParent() ? parent->distSinceFeature() : 0 );
}

float TreeSegment::distToFeature() {

    if( hasBranch() ) {
        float along = branches[0]->length_along_parent;
        for( int i = 1; i < branches.size(); ++i )
            along = fminf(along, branches[i]->length_along_parent);
        return along * length;
    }
    if( hasLeaf() ) {
        float along = leaves[0]->length_along_parent;
        for( int i = 1; i < leaves.size(); ++i )
            along = fminf(along, leaves[i]->length_along_parent);
        return along * length;
    }
    return length + ( ( stems.size() == 1 ) ? stems[0]->distToFeature() : 0 );
}

float TreeSegment::lastFeatureAngle() {
    
    if( hasBranch() ) {
        float angle = branches[0]->angle_roll;
        float dist = branches[0]->length_along_parent;
        for( int i = 1; i < branches.size(); ++i )
            if( branches[i]->length_along_parent > dist )
                angle = branches[i]->angle_roll;
        return angle;
    }
    if( hasLeaf() ) {
        float angle = leaves[0]->angle_roll;
        float dist = leaves[0]->length_along_parent;
        for( int i = 1; i < leaves.size(); ++i )
            if( leaves[i]->length_along_parent > dist )
                angle = leaves[i]->angle_roll;
        return angle;
    }
    return ( hasParent() && !(is_branch || is_split) ) ? parent->lastFeatureAngle() : roll_down;
}

void TreeSegment::setMaterial() {
    const float scale = powf( radius_1 / tree->max_segment_radius, tree->bark_material_exp);
    
    set_colour(lerp(scale, tree->colour_new, tree->colour_old));
    setTextureAlpha(lerp(scale, tree->texalpha_new, tree->texalpha_old));
    setBumpScale(lerp(scale, tree->bumpiness_new, tree->bumpiness_old));
    set_specular(vec4(lerp(scale, tree->specular_old, tree->specular_old)));
    
    enableTex(true);   bindTexture(tree->bark_texture, TEX_UNIT);
    enableBump(true);  bindTexture(tree->bark_normal, BUMP_UNIT);
    enableGloss(true); bindTexture(tree->bark_gloss, GLOSS_UNIT);
}

void TreeSegment::calculateDown() {

    vec3 local_up = mModel * vec4(0, 1, 0, 0);

    if( fabs(local_up.y) <= 0.99 ) {

        vec3 local_forward = mModel * vec4(1, 0, 0, 0);
        vec3 planar_left = normalize( cross(local_up, vec3(0, -1, 0) ) );
        vec3 planar_forward = cross(planar_left, local_up);

        float sign = ( dot(cross(planar_forward, local_forward), local_up) >= 0 ) ? 1 : -1;
        roll_down = sign * ( (180/3.14159f) * acosf( dot( planar_forward, local_forward ) ) );
    }
}

void TreeSegment::draw() {
    
    if( radius_1 < min_radius )
        return;
    
    setMaterial();
    
    mstack.push(mModel);
    
    // transform to our own coordinate space with pitch and roll
    if( is_branch )
        mModel *= Translate(0, parent->getLength() * length_along_parent, 0);
    mModel *= RotateY(angle_roll) * RotateZ(angle_pitch);
    
    // draw connecting wedge to parent
    if( fabs(angle_pitch) > 0.1 && !( is_split || is_branch ) )
        drawWedge(angle_roll, angle_pitch);
    
    // calculate the direction to allow down rotation
    if( roll_down == 0 )
        calculateDown();

    // draw our tapered cylinder
    mstack.push(mModel);
    mModel *= Translate(0, length/2, 0) * Scale(radius_1, length, radius_1) * RotateX(-90);
    tree->getSegmentShape(radius_2/radius_1, length, distance)->draw();
    mModel = mstack.pop();
    
    // draw leaves and branches
    for( int i = 0; i < leaves.size(); ++i )
        leaves[i]->draw();
    for( int i = 0; i < branches.size(); ++i)
        branches[i]->draw();

    // draw continuing stems, after translating to end of this segment
    mModel *= Translate(0, length, 0);
    if( stems.size() != 1 ) {
        mstack.push(mModel);
        mModel *= Scale(radius_2);
        drawSphere();
        mModel = mstack.pop();
    }
    for( int i = 0; i < stems.size(); ++i)
        stems[i]->draw();
    
    mModel = mstack.pop();
}

void TreeSegment::drawWedge(float roll, float pitch) {

    mstack.push(mModel);
    
    mModel *= Scale(radius_1) * RotateY(-90);
    if( pitch < 0 )
        mModel *= RotateY(180);
    mModel *= RotateX(-90);
    
    tree->getWedgeShape(fabs(pitch), distance)->draw();
    
    mModel = mstack.pop();
}

void TreeSegment::grow(float delt_radius) {
    float new_radius = radius_1 + delt_radius;
    grow(new_radius, tree->calcSlope(new_radius));
}

void TreeSegment::grow(float new_radius, float new_slope) {

    float delt_radius1 = new_radius - radius_1, delt_radius2 = 0;
    if( delt_radius1 <= 0 )
        return;
    
    // 1) calc new radius_1, radius_2, length
    slope = new_slope;
    radius_1 = new_radius;
    float new_length = ( radius_1 - min_radius ) * slope;
    if( new_length > max_length ) {
        length = max_length;
        delt_radius2 = radius_2;
        radius_2 = (new_length-length)/slope + min_radius;
        delt_radius2 = radius_2 - delt_radius2;
        
        // 2) create new stems to continue path
        if( stems.size() == 0 && distSinceLevel() < tree->genes[level].max_distance )
            generateNextStems();
        
        // 3) create new branches (probability)
        else if( branches.size() == 0 )//&& radius_1 - last_branch_check > tree->min_branch_check )
            tryToBranch();
    }
    else {
        length = new_length;
        radius_2 = min_radius;
    }
    
    // 4) generate leaves
    if( leaves.size() == 0 )
        tryToLeaf();
    
    // 5) grow all children
    for( int i = 0; i < stems.size(); ++i ) {
        if( hasSplit() )
            stems[i]->grow(delt_radius2);
        else
            stems[i]->grow(radius_2, new_slope);
    }
    for( int i = 0; i < branches.size() ; ++i )
        branches[i]->grow(delt_radius1);
    for( int i = 0; i < leaves.size(); ++i )
        leaves[i]->grow(radius_1);
}

void TreeSegment::generateNextStems() {
    bool make_split =
        level                 < tree->max_levels
        && distSinceLevel()   > tree->genes[level].min_split_distance
        && tree->getRandom()  < tree->genes[level].split_probability;
    if( make_split ) {
        const int num_split = tree->genes[level].num_splits;
        const float delt_roll = 360.0f / num_split;
        const float shift_roll = tree->randLerp(0, 180);

        for( int i = 0; i < num_split; ++i ) {
            const float stem_angle = tree->randLerp(tree->genes[level].min_split_angle, tree->genes[level].max_split_angle);
            
            TreeSegment* split = new TreeSegment(tree, this, length, false, true);
            split->angle_pitch += stem_angle;
            split->angle_roll  += shift_roll + i * delt_roll + tree->randLerp(0, 10);
            split->radius_1 = tree->randLerp(0, min_radius);
            
            stems.push_back(split);
        }
    }
    else {
        TreeSegment* next = new TreeSegment(tree, this, length, false, false);
        stems.push_back(next);
    }
}

void TreeSegment::tryToBranch() {
    bool make_branch =
        level                 < tree->max_levels
        && distSinceLevel()   > tree->genes[level].min_branch_distance
        && distSinceFeature() > tree->genes[level].min_branch_separation
        && distToFeature()    > tree->genes[level].min_branch_separation;
    if( make_branch ) {
        if( tree->getRandom() < tree->genes[level].branch_probability ) {
            float set_roll = tree->randLerp(tree->genes[level].min_branch_roll, tree->genes[level].max_branch_roll);
            TreeSegment* check = parent;
            while( check != NULL && !check->hasBranch() && !(check->is_branch || check->is_split) )
                check = check->parent;
            if( check->is_branch )
                set_roll += 90;
            
            const float along = 0.5f;
            TreeSegment* branch = new TreeSegment(tree, this, along, true, false);
            branch->angle_pitch += tree->randLerp(tree->genes[level].min_branch_angle, tree->genes[level].max_branch_angle);
            branch->angle_roll += set_roll + lastFeatureAngle() + 180;
            branches.push_back(branch);
        }
        else {
            last_branch_check = min_radius;
        }
    }
}

void TreeSegment::tryToLeaf() {
    bool make_leaf =
        distSinceLevel()      > tree->genes[level].min_leaf_distance
        && distSinceFeature() > tree->genes[level].min_leaf_separation
        && distToFeature()    > tree->genes[level].min_leaf_separation
        && tree->getRandom()  < tree->genes[level].leaf_probability
        && roll_down         != 0;
    if( make_leaf ) {
        const float last_feature_roll = lastFeatureAngle();
        for( int i = 0; i < tree->genes[level].num_leaves; ++i ) {
            TreeLeaf* leaf = new TreeLeaf(tree, this);
            leaf->angle_pitch = tree->randLerp(20, 45);
            leaf->length_along_parent = tree->randLerp(0.1, 0.9);
            leaf->angle_roll = tree->randLerp(tree->genes[level].min_leaf_roll, tree->genes[level].max_leaf_roll) + last_feature_roll + ((i+1)*180);
            leaves.push_back(leaf);
        }
    }
}



