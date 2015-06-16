#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "TextureManager.h"

std::string directory("../Images/");

GLuint loadTexture(const char *filename) {
    return loadTexture(filename, true, false);
}
GLuint loadTexture(const char *filename, bool mipmaps, bool clamp) {
    TgaImage image;
    GLuint texture;
    std::string location = directory + std::string(filename);
    
    if (!image.loadTGA(location.c_str())) {
        printf("Error loading image file '%s'\n", filename);
        exit(1);
    }
    
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp ? GL_CLAMP : GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp ? GL_CLAMP : GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    if( mipmaps ) {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }
    else
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );        
    glTexImage2D(GL_TEXTURE_2D, 0, 4, image.width, image.height, 0,
                 (image.byteCount == 1) ? GL_LUMINANCE : ( (image.byteCount == 3) ? GL_BGR : GL_BGRA ),
                 GL_UNSIGNED_BYTE, image.data );
    
    return texture;
}

bool TgaImage::loadTGA(const char *filename)
{
    FILE *file;
    unsigned char type[4];
    unsigned char info[6];
    
    file = fopen(filename, "rb");
    
    if (!file)
        return false;
    
    fread( &type, sizeof (char), 3, file );
    fseek( file, 12, SEEK_SET);
    fread( &info, sizeof (char), 6, file );
    
    //image type either 2 (color) or 3 (greyscale)
    if( type[1] != 0 || (type[2] != 2 && type[2] != 3) ) {
        printf("Error: Invalid image type\n");
        fclose(file);
        return false;
    }
    
    width = info[0] + info[1] * 256;
    height = info[2] + info[3] * 256;
    byteCount = info[4] / 8;
    
    if( byteCount != 1 && byteCount != 3 && byteCount != 4 ) {
        printf("Error: Invalid image bytecount %d\n", byteCount);
        fclose(file);
        return false;
    }
    
    long imageSize = width * height * byteCount;
    
    //allocate memory for image data
    data = new unsigned char[imageSize];
    
    //read in image data
    fread(data, sizeof(unsigned char), imageSize, file);
    
    //close file
    fclose(file);
    
    return true;
}
