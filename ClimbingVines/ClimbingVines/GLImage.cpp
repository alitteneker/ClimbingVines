//  GLImage.cpp
//  ClimbingVines

#include "GLImage.h"

GLImage::GLImage() {
    
}

GLImage::GLImage(int width, int height, unsigned char bytecount) {
    resize_image(width, height, bytecount);
}

GLImage::GLImage(int width, int height, unsigned char bytecount, unsigned char* data) {
    resize_image(width, height, Image::bytecount);
    data = data;
}

void GLImage::resize_image(int width, int height, unsigned char bytecount) {
    this->width = width;
    this->height = height;
    this->bytecount = bytecount;
    data = new unsigned char[ width * height * bytecount ];
}
