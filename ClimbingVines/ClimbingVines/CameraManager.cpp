
#include <stdio.h>
#include "CameraManager.h"

CameraManager camera;

CameraManager::CameraManager() {
    reset();
}
void CameraManager::reset() {
    eye = vec4{ 20.0, 3.7, 0.0, 1.0 };
    ref = vec4{ 0.0, 2.7, 0.0,   1.0 };
    up  = vec4{ 0.0, 1.0, 0.0,   0.0 };
}
void CameraManager::setEye(vec4 set) {
    eye = set;
}
void CameraManager::setRef(vec4 set) {
    ref = set;
}
void CameraManager::setUp(vec4 set) {
    up = normalize(set);
}
void CameraManager::rotateAroundHorizontal(float delt) {
    vec4 dist = eye - ref;
    mat4 rotate = RotateAxis(delt, up);
    eye = ref + ( dist * rotate );
}
void CameraManager::rotateAroundVertial(float delt) {
    vec4 dist = eye - ref;
    vec4 left = cross(normalize(dist), up);
    mat4 rotate = RotateAxis(delt, left);
    up = up * rotate;
    eye = ref + ( dist * rotate );
}
void CameraManager::rotateAroundRoll(float delt) {
    up *= RotateAxis(delt, normalize(eye-ref));
}
void CameraManager::changeAroundDistance(float delt) {
    vec4 dist = eye - ref;
    float len = length(dist);
    if( len + delt > 0 )
        eye = ref + ( dist * ( ( len + delt ) / len ) );
}