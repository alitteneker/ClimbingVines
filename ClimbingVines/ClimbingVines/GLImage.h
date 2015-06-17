//  GLImage.h
//  ClimbingVines

#ifndef __ClimbingVines__GLImage__
#define __ClimbingVines__GLImage__

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
#include "Image.h"

class GLImage : public Image {
public:
    unsigned char *data;
    GLuint binding;
    
    GLImage();
    GLImage(int width, int height, unsigned char bytecount);
    GLImage(int width, int height, unsigned char bytecount, unsigned char* data);
    void resize_image(int width, int height, unsigned char bytecount);
};

#endif
