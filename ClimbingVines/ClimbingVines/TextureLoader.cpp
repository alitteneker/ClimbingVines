#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
using namespace std;

#include "TextureLoader.h"

string directory("../Assets/");

GLImage* loadTexture(const string filename) {
    return loadTexture(filename.c_str(), true, false);
}
GLImage* loadTexture(const char *filename) {
    return loadTexture(filename, true, false);
}
GLImage* loadTexture(const char *filename, bool mipmaps, bool clamp) {
    std::string location = directory + std::string(filename);
    
    GLImage *image = GLloadTGA(location.c_str());
    if( image == NULL ) {
        printf("Error loading image file '%s'\n", filename);
        exit(1);
    }
    bindTexture(image, mipmaps, clamp);
    
    return image;
}

void bindTexture(GLImage *image, bool mipmaps, bool clamp) {

    glGenTextures( 1, &(image->binding) );
    glBindTexture( GL_TEXTURE_2D, image->binding );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP : GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP : GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    if( mipmaps ) {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }
    else
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, image->width, image->height, 0,
                 (image->bytecount == 1) ? GL_LUMINANCE : ( (image->bytecount == 3) ? GL_BGR : GL_BGRA ),
                 GL_UNSIGNED_BYTE, image->data );
}

map<string, GLImage*> GL_loaded;

GLImage* GLloadTGA(const string filename) {
    
    if( GL_loaded.find(filename) != GL_loaded.end() )
        return GL_loaded[filename];
    
    FILE *file = fopen(filename.c_str(), "rb");
    
    if (!file)
        return NULL;
    
    unsigned char type[4];
    unsigned char info[6];
    
    fread( &type, sizeof (char), 3, file );
    fseek( file, 12, SEEK_SET);
    fread( &info, sizeof (char), 6, file );
    
    //image type either 2 (color) or 3 (greyscale)
    if( type[1] != 0 || (type[2] != 2 && type[2] != 3) ) {
        printf("Error: Invalid image type\n");
        fclose(file);
        return NULL;
    }
    
    GLImage *ret = new GLImage();
    ret->resize_image( info[0] + info[1] * 256, info[2] + info[3] * 256, info[4] / 8 );
    
    if( ret->bytecount != 1 && ret->bytecount != 3 && ret->bytecount != 4 ) {
        printf("Error: Invalid image bytecount %d\n", ret->bytecount);
        fclose(file);
        return NULL;
    }
    
    //allocate memory for image data
    long imageSize = ret->width * ret->height * ret->bytecount;
    ret->data = new unsigned char[imageSize];
    
    //read in image data
    fread(ret->data, sizeof(unsigned char), imageSize, file);
    
    //close file
    fclose(file);
    
    return ret;
}
