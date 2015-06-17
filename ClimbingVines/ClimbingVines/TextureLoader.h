#include <stdio.h>

#ifndef _TreeTest_TextureLoader_
#define _TreeTest_TextureLoader_

#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include "GLImage.h"

extern string directory;

GLImage* loadTexture(const string filename);
GLImage* loadTexture(const char *filename);
GLImage* loadTexture(const char *filename, bool mipmaps, bool clamp);
void bindTexture(GLImage *image, bool mipmaps, bool clamp);
GLImage* GLloadTGA(const string filename);

#endif
