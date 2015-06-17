//  GLOBJParser.cpp
//  ClimbingVines

#include "GLOBJParser.h"

#include "ObjParser.h"
#include "PTriangle.h"
#include <iostream>
#include <fstream>
#include <sstream>

void GL_parse_mtl_file(string filename, map<string, Material*>& existing) {
    
    Material* curr = NULL;
    
    ifstream is = open_file(directory + filename);
    vector<string> vs;
    while( !is.eof() ) {
        get_line_elements(vs, is);
        if( vs.size() == 0 || vs[0] == "" || vs[0].at(0) == '#' )
            continue;
        
        if( vs[0] == "newmtl" ) {
            curr = new Material();
            curr->name = vs[1];
            existing[vs[1]] = curr;
        }
        else if( vs[0] == "Ka" ) {
            curr->k_ambient = vec4( stof(vs[1]), stof(vs[2]), stof(vs[3]), 1 );
        }
        else if( vs[0] == "Kd" ) {
            curr->k_diffuse = vec4( stof(vs[1]), stof(vs[2]), stof(vs[3]), 1 );
        }
        else if( vs[0] == "Ks" ) {
            curr->k_specular = vec4( stof(vs[1]), stof(vs[2]), stof(vs[3]), 1 );
        }
        else if( vs[0] == "Ns" ) {
            curr->n_specular = stof(vs[1]);
        }
        else if( vs[0] == "Tr" || vs[0] == "d" ) {
            // apparently both keys are used in different implementations
            curr->transparency = stof(vs[1]);
        }
        else if( vs[0] == "Tf" ) {
            curr->transmission_f = vec4( stof(vs[1]), stof(vs[2]), stof(vs[3]), 1 );
        }
        else if( vs[0] == "Ni" ) {
            curr->refractive_index = stof(vs[1]);
        }
        else if( vs[0] == "Kr" ) {
            // this is one I'm making up to match the original format: reflection coeff
            // there must be a solution to this problem in the standard I'm just too tired to see it right now
            curr->k_reflect = stof(vs[1]);
        }
        else if( vs[0] == "map_Ka" ) {
            curr->map_Ka = loadTexture(vs[1]);
        }
        else if( vs[0] == "map_Kd" ) {
            curr->map_Kd = loadTexture(vs[1]);
        }
        else if( vs[0] == "map_Ks" ) {
            curr->map_Ks = loadTexture(vs[1]);
        }
        else if( vs[0] == "map_Ns" ) {
            curr->map_Ns = loadTexture(vs[1]);
        }
        else if( vs[0] == "map_Tr" || vs[0] == "map_d" ) {
            curr->map_Tr = loadTexture(vs[1]);
        }
        else if( vs[0] == "map_refl" || vs[0] == "refl" ) {
            curr->map_refl = loadTexture(vs[1]);
        }
        else if( vs[0] == "map_bump" || vs[0] == "bump" ) {
            curr->map_bump = loadTexture(vs[1]);
        }
        else if( vs[0] == "illum" ) {
            // I'm not sure I really want to tackle this one. It's messy.
        }
        else {
            cout << "Warning: Unknown pattern \""+vs[0]+"\" detected while attempting to parse mtl file \""+filename+"\".\n";
        }
    }
}

vector<Primitive*> GL_parse_obj_file(string filename) {
    vector<Primitive*> ret;
    
    vector<vec4> vertices;
    vector<vec3> textures;
    vector<vec4> normals;
    map<string, Material*> materials;
    
    string group_name = "";
    string obj_name = "";
    Material* curr_mtl = NULL;
    
    ifstream is = open_file(directory + filename);
    vector<string> vs;
    while( !is.eof() ) {
        get_line_elements(vs, is);
        if( vs.size() == 0 || vs[0] == "" || vs[0].at(0) == '#' )
            continue;
        
        if( vs[0] == "v" ) {
            // vertex: v x y z [w def 1]
            vec4 add(stof(vs[1]), stof(vs[2]), stof(vs[3]), 1);
            if( vs.size() > 4 && vs[4] != "" )
                add.w = stof(vs[4]);
            vertices.push_back(add);
        }
        else if( vs[0] == "vt" ) {
            // texture coord: vt x y [w def 1]
            vec3 add(stof(vs[1]), stof(vs[2]), 1);
            if( vs.size() > 3 && vs[3] != "" )
                add.z = stof(vs[3]);
            textures.push_back(add);
        }
        else if( vs[0] == "vn" ) {
            // normal: v x y z [w def 0]
            vec4 add(stof(vs[1]), stof(vs[2]), stof(vs[3]), 0);
            if( vs.size() > 4 && vs[4] != "" )
                add.w = stof(vs[4]);
            normals.push_back(normalize(add));
        }
        else if( vs[0] == "f" ) {
            // face (treat more than 3 vertices as triangle fan for simplicity)
            if( vs.size() < 4 || vs[3] == "" ) {
                cout << "Cannot parse face with less than three elements.\n";
                exit(1);
            }
            
            // ----- OVERVIEW OF BELOW -----
            // face (treat more than 3 vertices as triangle fan for simplicity)
            // f 1 -2 34 . . .         # 3+ vertex indices with base 1, negative indices are end-relative (-1 is last element)
            // f 1/1 100/-1 -2/25 ...  # with texture coordinate indices
            // f 1/1/1 2/2/2 3/3/3 ... # with texture and normal indices
            // f 1//1 2//2 3//3 ...    # can also have normal indices without texture coords
            
            vec4 root_v, first_v, root_n, first_n, next_v, next_n;
            vec3 root_t, first_t, next_t;
            bool has_tex = false, has_norm = false;
            
            // parse the root of the fan
            vector<string> *fe = split(vs[1], '/');
            unsigned long element_count = fe->size();
            root_v = vertices.at( get_index(stol(fe->at(0)), vertices.size()) );
            if( element_count > 1 && fe->at(1) != "" ) {
                has_tex = true;
                root_t = textures.at( get_index(stol(fe->at(1)), textures.size()) );
            }
            if( element_count > 2 && fe->at(2) != "" ) {
                has_norm = true;
                root_n = normals.at( get_index(stol(fe->at(2)), normals.size()) );
            }
            
            int index;
            for( index = 2; index < vs.size() && vs[index] != ""; ++index ) {
                // parse the next node, which allows us to generate a triangle fan
                fe = split(vs[index], '/');
                if( fe->size() != element_count ) {
                    cout << "Cannot have variable numbers of parameters on the same face.\n";
                    exit(1);
                }
                next_v = vertices.at( get_index(stol(fe->at(0)), vertices.size()) );
                if( element_count > 1 && fe->at(1) != "" )
                    next_t = textures.at( get_index(stol(fe->at(1)), textures.size()) );
                if( element_count > 2 && fe->at(2) != "" )
                    next_n = normals.at( get_index(stol(fe->at(2)), normals.size()) );
                
                if( index > 2 ) {
                    PTriangle *add = new PTriangle();
                    
                    add->material = curr_mtl;
                    
                    add->points[0] = root_v; add->points[1] = first_v; add->points[2] = next_v;
                    add->init();
                    
                    if(has_norm) { add->normals[0] = root_n; add->normals[1] = first_n; add->normals[2] = next_n; }
                    else         { add->normals[0] = add->normals[1] = add->normals[2] = ((Plane*)add)->normal; }
                    
                    if(has_tex) {
                        add->uv_coorinates[0]=root_t; add->uv_coorinates[1]=first_t; add->uv_coorinates[2]=next_t;
                        
                        vec2 deltaUV1 = add->uv_coorinates[1] - add->uv_coorinates[0],
                        deltaUV2 = add->uv_coorinates[2] - add->uv_coorinates[0];
                        vec4 bitangent = deltaUV1.x*(add->points[2]-add->points[0]) - deltaUV2.x*(add->points[1]-add->points[0]);
                        
                        float r = deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x;
                        if( fabsf(r) > 0.0001f ) {
                            bitangent /= r;
                            add->tangents[0] = normalize( cross( add->normals[0], bitangent ) );
                            add->tangents[1] = normalize( cross( add->normals[1], bitangent ) );
                            add->tangents[2] = normalize( cross( add->normals[2], bitangent ) );
                        }
                    }
                    
                    ret.push_back(add);
                }
                
                first_v = next_v;
                first_t = next_t;
                first_n = next_n;
            }
        }
        else if( vs[0] == "mtllib" ) {
            GL_parse_mtl_file(vs[1], materials);
        }
        else if( vs[0] == "usemtl" ) {
            if( materials.find(vs[1]) == materials.end() )
                Error("Unknown mtl \""+vs[1]+"\" found.");
            curr_mtl = materials[vs[1]];
        }
        else if( vs[0] == "o" ) {
            obj_name = vs[1];
        }
        else if( vs[0] == "g" ) {
            group_name = vs[1];
        }
        else if( vs[0] == "s" ) {
            // Not quite sure how this really works
        }
        else {
            cout << "Unknown pattern in obj file.\n";
            exit(1);
        }
    }
    
    return ret;
}
