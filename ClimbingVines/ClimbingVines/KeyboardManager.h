
#ifndef __TreeTest__KeyboardManager__
#define __TreeTest__KeyboardManager__

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

#include <stdio.h>
#include <vector>

// Allows registered listeners for keyDown events, and buffers key states
class KeyboardManager {
protected:
    std::vector<void (*)(unsigned char)> pressedListeners;
    std::vector<void (*)(unsigned char)> specialPressedListeners;
public:
    bool* keyStates = new bool[256];
    bool* keySpecialStates = new bool[256];
    void init();
    bool handleKeys();
    void markKeyDown(unsigned char key) {
        keyStates[key] = true;
        for( int i = 0; i < pressedListeners.size(); ++i )
            pressedListeners[i](key);
    }
    void markKeyUp(unsigned char key) {
        keyStates[key] = false;
    }
    void markSpecialDown(int key) {
        keySpecialStates[key] = true;
        for( int i = 0; i < specialPressedListeners.size(); ++i )
            specialPressedListeners[i](key);
    }
    void markSpecialUp(int key) {
        keySpecialStates[key] = false;
    }
    void addKeyPressedListener(void (*listen)(unsigned char)) {
        pressedListeners.push_back(listen);
    }
    void addSpecialKeyPressedListener(void (*listen)(unsigned char)) {
        specialPressedListeners.push_back(listen);
    }
};

extern KeyboardManager keyManager;

#endif
