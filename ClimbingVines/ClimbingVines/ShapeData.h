#ifndef _SHAPES_H_
#define _SHAPES_H_

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#ifdef WIN32
#include "GL/freeglut.h"
#else
#include <GLUT/glut.h>
#endif

#include "matm.h"

class ShapeData {
public:
    ShapeData(int size);
    ~ShapeData();
    
    void bind( GLuint program );
    bool enableTangentArray( GLuint program, bool set );
    bool enableUVArray( GLuint program, bool set );
    bool draw();
    bool subPoints();
    bool subNormals();
    bool subTangents();
    bool subUVs();
    
    GLuint vao = 0;
    int numVertices = 0;
    vec4* points   = 0;
    vec3* normals  = 0;
    vec3* tangents = 0;
    vec2* UVs      = 0;
protected:
    void setVertexAttrib(GLuint program,
                         GLfloat* p,  GLsizeiptr ps,
                         GLfloat* n,  GLsizeiptr ns,
                         GLfloat* t,  GLsizeiptr ts,
                         GLfloat* tx, GLsizeiptr txs);
    bool enableTangents = false;
    bool enableUVs = false;
    GLuint point_buffer, norm_buffer, tang_buffer, tex_buffer;
};

void drawShape(ShapeData* shape);

#endif