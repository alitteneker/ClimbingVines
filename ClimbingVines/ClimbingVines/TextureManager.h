#include <stdio.h>

#ifndef _TreeTest_TextureManager_
#define _TreeTest_TextureManager_

#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

class TgaImage
{
    public:
        TgaImage()
        {
            data = (unsigned char*)0;
            width = 0;
            height = 0;
            byteCount = 0;
        }

        ~TgaImage() { delete[] data; data = 0; }

        bool loadTGA(const char* filename);

        int width;
        int height;
        unsigned char byteCount;
        unsigned char* data;
};

GLuint loadTexture(const char *filename);
GLuint loadTexture(const char *filename, bool mipmaps, bool clamp);

#endif
