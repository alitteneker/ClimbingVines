
#ifndef __TreeTest__CameraManager__
#define __TreeTest__CameraManager__

#include <stdio.h>
#include "matm.h"
#include "ShaderManager.h"
#include "MatrixManager.h"

class CameraManager {
public:
    CameraManager();
    void rotateAroundHorizontal(float delt);
    void rotateAroundVertial(float delt);
    void rotateAroundRoll(float delt);
    void changeAroundDistance(float delt);
    void setEye(vec4 set);
    void setRef(vec4 set);
    void setUp(vec4 set);
    vec4 getEye() { return eye; }
    vec4 getRef() { return ref; }
    vec4 getUp()  { return up;  }
    void reset();
    void setVars() {
        mView = LookAt(eye, ref, up);
        writeView();
        set_camera_position(eye);
    }
protected:
    vec4 eye;
    vec4 ref;
    vec4 up;
};

extern CameraManager camera;

#endif
