#ifndef __TreeTest__ShaderManager__
#define __TreeTest__ShaderManager__

#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <stdio.h>
#include "matm.h"

extern const int TEX_UNIT;
extern const int BUMP_UNIT;
extern const int GLOSS_UNIT;

GLuint InitShader( const char* vertexShaderFile, const char* fragmentShaderFile );

void initShaderVars(GLuint program);
void bindTexture(GLuint texture, GLuint unit);
void writeModel();
void writeModel(mat4 model);
void writeView();
void writeView(mat4 view);
void writeProjection(mat4 projection);
void writeOrtho(mat4 ortho);
void enableOrtho(bool enable);
void enableLighting(bool enable);
void enableTex(bool enable);
void enableBump(bool enable);
void enableGloss(bool enable);
void setBumpScale(float set);
void setTextureAlpha(float set);
void setMinDiffuse(float set);
void set_light_position(vec4 set);
void set_light_position(float x, float y, float z);
void set_camera_position(vec4 set);
void enableEyeRelative(bool enable);
void set_ambient(float s);
void set_diffuse(float s);
void set_specular(float s);
void set_shininess(float s);
void set_colour(float r, float g, float b);
void set_colour(vec4 set);
void set_material(float a, float d, float s);
void writeColour();
void writeMaterial();

#endif
