//  PlantVoxelMap.h
//  ClimbingVines

#ifndef __ClimbingVines__PlantVoxelMap__
#define __ClimbingVines__PlantVoxelMap__

#include <stdio.h>
#include "VoxelMap.h"

class PlantVoxel : public Voxel {
public:
    
    vec4 closest_center = vec4(INFINITY, 1);
    
    PlantVoxel(vec4 min, vec4 max, VoxelMap *parent);
    void draw();
    
};

class PlantVoxelMap : public VoxelMap {
public:
    
    PlantVoxelMap(const vec4 &min, const vec4& max, const float size, const int x_size, const int y_size, const int z_size);
    PlantVoxelMap(const float size, const float edge, vector<Shape*> &shapes);
    PlantVoxelMap(const float size, const float edge, vector<Primitive*> &shapes);
    void resize(const float size, const int x_size, const int y_size, const int z_size);
    
    void _recurse_closest(const vec4 &center, vec3 loc, vec3 dir);
    void calc_closest();
    void draw();
    
};

#endif
