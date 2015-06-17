//  GLOBJParser.h
//  ClimbingVines

#ifndef __ClimbingVines__GLOBJParser__
#define __ClimbingVines__GLOBJParser__

#include <stdio.h>
#include "TextureLoader.h"
#include "Primitive.h"
#include <map>

void GL_parse_mtl_file(string filename, map<string, Material*>& existing);
vector<Primitive*> GL_parse_obj_file(string filename);

#endif
