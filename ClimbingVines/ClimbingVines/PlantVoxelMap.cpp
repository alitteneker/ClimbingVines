//
//  PlantVoxelMap.cpp
//  ClimbingVines
//
//  Created by Alan Litteneker on 6/17/15.
//  Copyright (c) 2015 Alan Litteneker. All rights reserved.
//

#include "PlantVoxelMap.h"
#include "Primitives.h"
#include "MatrixManager.h"
#include "ShaderManager.h"

PlantVoxel::PlantVoxel(vec4 min, vec4 max, VoxelMap *parent) : Voxel(min, max, parent) {}

void PlantVoxel::draw() {
    if( is_empty() )
        return;
    
    mstack.push(mModel);
    mModel *= Translate(get_center()) * Scale(get_size());
    drawCube();
    mModel = mstack.pop();
}

PlantVoxelMap::PlantVoxelMap(const vec4 &min, const vec4& max, const float size, const int x_size, const int y_size, const int z_size) : VoxelMap(min, max, size, x_size, y_size, z_size) {}
PlantVoxelMap::PlantVoxelMap(const float size, const float edge, vector<Shape*> &shapes) : VoxelMap(size, edge, shapes) {}
PlantVoxelMap::PlantVoxelMap(const float size, const float edge, vector<Primitive*> &shapes) : VoxelMap(size, edge, shapes) {}

void PlantVoxelMap::resize(const float size, const int x_size, const int y_size, const int z_size) {
    this->unit_size = size;
    this->x_size = x_size;
    this->y_size = y_size;
    this->z_size = z_size;
    
    voxels.resize( x_size * y_size * z_size);
    
    for( int x = 0; x < x_size; ++x ) {
        for( int y = 0; y < y_size; ++y ) {
            for( int z = 0; z < z_size; ++z ) {
                vec4 offset = min + vec4(x*size, y*size, z*size, 0);
                voxels[get_index(x, y, z)] = new PlantVoxel( offset, offset + vec4(size, 0), this );
            }
        }
    }
}

void PlantVoxelMap::draw() {
    enableBump(false); enableGloss(false);
    set_colour(0.9, 0.9, 0.9);
    set_diffuse(1.0);
    for( int i = 0; i < voxels.size(); ++i )
        ((PlantVoxel*)voxels[i])->draw();
}

void PlantVoxelMap::_recurse_closest(const vec4 &center, vec3 loc, vec3 dir) {
    
    if( loc.x < 0 || loc.x == x_size || loc.y < 0 || loc.y == y_size || loc.z < 0 || loc.z == z_size )
        return;
    
    PlantVoxel *vox = (PlantVoxel*)get_voxel(loc.x, loc.y, loc.z);
    if( vox->is_empty() && length_squared(vox->get_center() - center) < length_squared(vox->get_center() - vox->closest_center) )
        vox->closest_center = center;
    else if( length_squared(dir) > 0 )
        return;
    
    for( int x = -1; x <= 1; ++x ) {
        for( int y = -1; y <= 1; ++y ) {
            for( int z = -1; z <= 1; ++z ) {
                if( x == 0 && y == 0 && z == 0 )
                    continue;
                if( ( dir.x != 0 && x != dir.x ) || ( dir.y != 0 && y != dir.y ) || ( dir.z != 0 && z != dir.z ) )
                    continue;
                
                vec3 dir(x,y,z);
                _recurse_closest(center, loc + dir, dir);
            }
        }
    }
}

void PlantVoxelMap::calc_closest() {
    for( int x = 0; x < x_size; ++x ) {
        for( int y = 0; y < y_size; ++y ) {
            for( int z = 0; z < z_size; ++z ) {
                PlantVoxel *vox = (PlantVoxel*)get_voxel(x, y, z);
                if( !vox->is_empty() ) {
                    vox->closest_center = vox->get_center();
                    _recurse_closest(vox->get_center(), vec3(x,y,z), vec3());
                }
            }
        }
    }
}
