

#include <stdio.h>
#include "KeyboardManager.h"
#include "CameraManager.h"

KeyboardManager keyManager;

bool KeyboardManager::handleKeys() {
    bool ret = false;
    float vert = 0, horiz = 0, roll = 0, dist = 0, bump = 1;
    if( keyManager.keySpecialStates[GLUT_KEY_UP] )
        vert += 1;
    if( keyManager.keySpecialStates[GLUT_KEY_DOWN] )
        vert -= 1;
    if( keyManager.keySpecialStates[GLUT_KEY_LEFT] )
        horiz -= 1;
    if( keyManager.keySpecialStates[GLUT_KEY_RIGHT] )
        horiz += 1;
    if( keyManager.keyStates['['] )
        roll -= 1;
    if( keyManager.keyStates[']'] )
        roll += 1;
    if( keyManager.keyStates['-'] )
        dist -= 0.1;
    if( keyManager.keyStates['='] )
        dist += 0.1;
    if( vert != 0 ) {
        camera.rotateAroundVertial(vert);
        ret = true;
    }
    if( horiz != 0 ) {
        camera.rotateAroundHorizontal(horiz);
        ret = true;
    }
    if( roll != 0 ) {
        camera.rotateAroundRoll(roll);
        ret = true;
    }
    if( dist != 0 ) {
        camera.changeAroundDistance(dist);
        ret = true;
    }
    return ret;
}

void keyDown(unsigned char key, int x, int y) {
    keyManager.markKeyDown(key);
}
void keyUp(unsigned char key, int x, int y) {
    keyManager.markKeyUp(key);
}
void keySpecialDown(int key, int x, int y) {
    keyManager.markSpecialDown(key);
}
void keySpecialUp(int key, int x, int y) {
    keyManager.markSpecialUp(key);
}

void KeyboardManager::init() {
    for( int i = 0; i < 256; ++i ) {
        keyStates[i] = false;
        keySpecialStates[i] = false;
    }
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(keySpecialDown);
    glutSpecialUpFunc(keySpecialUp);
}

