

#include "ShapeData.h"
#include "matm.h"
#include "ShaderManager.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

ShapeData::ShapeData(unsigned long size) {
    numVertices = size;
    points   = (vec4*)malloc(sizeof(vec4) * size);
    normals  = (vec3*)malloc(sizeof(vec3) * size);
    tangents = (vec3*)malloc(sizeof(vec3) * size);
    UVs      = (vec2*)malloc(sizeof(vec3) * size);
}
ShapeData::~ShapeData() {
    if( points )   free(points);
    if( normals )  free(normals);
    if( tangents ) free(tangents);
    if( UVs )      free(UVs);
}
void ShapeData::bind(GLuint program) {
    setVertexAttrib(program,
                    (float*)points,                        sizeof(vec4) * numVertices,
                    (float*)normals,                       sizeof(vec3) * numVertices,
                    enableTangents ? (float*)tangents : 0, sizeof(vec3) * numVertices,
                    enableUVs      ? (float*)UVs      : 0, sizeof(vec2) * numVertices);
}

void ShapeData::setVertexAttrib(GLuint program,
                     GLfloat* points,    GLsizeiptr psize,
                     GLfloat* normals,   GLsizeiptr nsize,
                     GLfloat* tangents,  GLsizeiptr tasize,
                     GLfloat* texcoords, GLsizeiptr tsize) {
    
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glGenBuffers( 1, &point_buffer );
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glBindBuffer( GL_ARRAY_BUFFER, point_buffer );
    glBufferData( GL_ARRAY_BUFFER, psize, points, GL_STREAM_DRAW );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    glGenBuffers( 1, &norm_buffer );
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glBindBuffer( GL_ARRAY_BUFFER, norm_buffer );
    glBufferData( GL_ARRAY_BUFFER, nsize, normals, GL_STREAM_DRAW );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    if( tangents ) {
        glGenBuffers( 1, &tang_buffer );
        glBindBuffer( GL_ARRAY_BUFFER, tang_buffer );
        glBufferData( GL_ARRAY_BUFFER, nsize, tangents, GL_STREAM_DRAW );
        GLuint vTangent = glGetAttribLocation( program, "vTangent" );
        glEnableVertexAttribArray( vTangent );
        glVertexAttribPointer( vTangent, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    }
    
    if( texcoords ) {
        glGenBuffers( 1, &tex_buffer );
        glBindBuffer( GL_ARRAY_BUFFER, tex_buffer );
        glBufferData( GL_ARRAY_BUFFER, tsize, texcoords, GL_STREAM_DRAW );
        GLuint vTexCoords = glGetAttribLocation( program, "vTexCoords" );
        glEnableVertexAttribArray( vTexCoords );
        glVertexAttribPointer( vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
}

bool ShapeData::enableTangentArray( GLuint program, bool set ) {
    if( set == enableTangents )
        return false;
    
    enableTangents = set;
    if( !vao )
        return true;
    
    glBindVertexArray( vao );
    GLuint vTangent = glGetAttribLocation( program, "vTangent" );

    if( !set ) {
        glDisableVertexAttribArray(vTangent);
        glDeleteBuffers(1, &tang_buffer);
    }
    else {
        glGenBuffers( 1, &tang_buffer );
        glBindBuffer( GL_ARRAY_BUFFER, tang_buffer );
        glBufferData( GL_ARRAY_BUFFER, sizeof(vec3)*numVertices, tangents, GL_STREAM_DRAW );
        glEnableVertexAttribArray( vTangent );
        glVertexAttribPointer( vTangent, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
    
    return true;
}
bool ShapeData::enableUVArray( GLuint program, bool set ) {
    if( set == enableUVs )
        return false;
    
    enableUVs = set;
    if( !vao )
        return true;
    
    glBindVertexArray( vao );
    GLuint vTexCoords = glGetAttribLocation( program, "vTexCoords" );
    
    if( !set ) {
        glDisableVertexAttribArray(vTexCoords);
        glDeleteBuffers(1, &tex_buffer);
    }
    else {
        glGenBuffers( 1, &tex_buffer );
        glBindBuffer( GL_ARRAY_BUFFER, tex_buffer );
        glBufferData( GL_ARRAY_BUFFER, sizeof(vec2)*numVertices, UVs, GL_STREAM_DRAW );
        glEnableVertexAttribArray( vTexCoords );
        glVertexAttribPointer( vTexCoords, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
    
    return true;
}
bool ShapeData::subPoints() {
    if( !vao )
        return false;
    
    glBindVertexArray( vao );
    glBindBuffer(GL_ARRAY_BUFFER, point_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4) * numVertices, points);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
    
    return true;
}
bool ShapeData::subNormals() {
    if( !vao )
        return false;
    
    glBindVertexArray( vao );
    glBindBuffer(GL_ARRAY_BUFFER, norm_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * numVertices, normals);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
    
    return true;
}
bool ShapeData::subTangents() {
    if( !vao || !enableTangents )
        return false;
    
    glBindVertexArray( vao );
    glBindBuffer(GL_ARRAY_BUFFER, tang_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * numVertices, tangents);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
    
    return true;
}
bool ShapeData::subUVs() {
    if( !vao || !enableUVs )
        return false;
    
    glBindVertexArray( vao );
    glBindBuffer(GL_ARRAY_BUFFER, tex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * numVertices, UVs);
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
    
    return true;
}
bool ShapeData::draw() {
    if(!vao)
        return false;
    drawShape(this);
    return true;
}

void drawShape(ShapeData* shape) {
    writeModel();
    glBindVertexArray( shape->vao );
    glDrawArrays( GL_TRIANGLES, 0, shape->numVertices );
    glBindVertexArray( 0 );
}

