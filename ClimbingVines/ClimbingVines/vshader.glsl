#version 120

attribute vec4 vPosition;
attribute vec3 vNormal, vTangent;
attribute vec2 vTexCoords;

uniform int EnableOrtho, EyeRelative;
uniform mat4 Model, View, Projection, Orthographic;
uniform vec4 LightPosition, CameraPosition;

// output values that will be interpolated per-fragment
varying vec3 fN, fT, fE, fL;

mat4 inverse(mat4 m) {
    float
    a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
    a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
    a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
    a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],
    
    b00 = a00 * a11 - a01 * a10,
    b01 = a00 * a12 - a02 * a10,
    b02 = a00 * a13 - a03 * a10,
    b03 = a01 * a12 - a02 * a11,
    b04 = a01 * a13 - a03 * a11,
    b05 = a02 * a13 - a03 * a12,
    b06 = a20 * a31 - a21 * a30,
    b07 = a20 * a32 - a22 * a30,
    b08 = a20 * a33 - a23 * a30,
    b09 = a21 * a32 - a22 * a31,
    b10 = a21 * a33 - a23 * a31,
    b11 = a22 * a33 - a23 * a32,
    
    det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    
    return mat4(a11 * b11 - a12 * b10 + a13 * b09,
                a02 * b10 - a01 * b11 - a03 * b09,
                a31 * b05 - a32 * b04 + a33 * b03,
                a22 * b04 - a21 * b05 - a23 * b03,
                a12 * b08 - a10 * b11 - a13 * b07,
                a00 * b11 - a02 * b08 + a03 * b07,
                a32 * b02 - a30 * b05 - a33 * b01,
                a20 * b05 - a22 * b02 + a23 * b01,
                a10 * b10 - a11 * b08 + a13 * b06,
                a01 * b08 - a00 * b10 - a03 * b06,
                a30 * b04 - a31 * b02 + a33 * b00,
                a21 * b02 - a20 * b04 - a23 * b00,
                a11 * b07 - a10 * b09 - a12 * b06,
                a00 * b09 - a01 * b07 + a02 * b06,
                a31 * b01 - a30 * b03 - a32 * b00,
                a20 * b03 - a21 * b01 + a22 * b00) / det;
}

mat4 transpose(in mat4 inMatrix) {
    vec4
        i0 = inMatrix[0],
        i1 = inMatrix[1],
        i2 = inMatrix[2],
        i3 = inMatrix[3];
    
    return mat4(
              vec4(i0.x, i1.x, i2.x, i3.x),
              vec4(i0.y, i1.y, i2.y, i3.y),
              vec4(i0.z, i1.z, i2.z, i3.z),
              vec4(i0.w, i1.w, i2.w, i3.w) );
}

void main() {
    // If enabled, make the object coordinates eye relative (eg. for a skybox)
    mat4 EyeTranslation = mat4(1.0);
    if( EyeRelative == 1 )
        EyeTranslation[3] = CameraPosition;
    mat4 ModelView = View * EyeTranslation * Model;

    vec4 N = vec4(normalize(vNormal), 0.0f);
    vec4 T = vec4((length(vTangent) > 0) ? normalize(vTangent) : vTangent, 0.0f);

    // should be inverse(transpose(ModelView))*N, but that requires version 150 or higher.
    fN =  normalize(inverse(transpose(ModelView)) * N).xyz;
    fT =  normalize(ModelView * T).xyz;
    fE = -(ModelView * vPosition).xyz;
    fL =  (View * LightPosition).xyz;

    if( LightPosition.w != 0.0f)
        fL = LightPosition.xyz - vPosition.xyz;

    gl_Position = ( EnableOrtho == 1 ? Orthographic : Projection ) * ModelView * vPosition;
    gl_TexCoord[0].xy = vTexCoords;
}

