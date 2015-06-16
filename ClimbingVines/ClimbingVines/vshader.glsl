#version 120

attribute vec4 vPosition;
attribute vec3 vNormal, vTangent;
attribute vec2 vTexCoords;

uniform int EnableOrtho, EyeRelative;
uniform mat4 Model, View, Projection, Orthographic;
uniform vec4 LightPosition, CameraPosition;

// output values that will be interpolated per-fragment
varying vec3 fN, fT, fE, fL;

void main() {
    // If enabled, make the object coordinates eye relative (eg. for a skybox)
    mat4 EyeTranslation = mat4(1.0);
    if( EyeRelative == 1 )
        EyeTranslation[3] = CameraPosition;
    mat4 ModelView = View * EyeTranslation * Model;

    vec4 N = vec4(normalize(vNormal), 0.0f);
    vec4 T = vec4((length(vTangent) > 0) ? normalize(vTangent) : vTangent, 0.0f);

    // should be inverse(transpose(ModelView))*N, but that requires version 150 or higher.
    fN =  normalize(ModelView * N).xyz;
    fT =  normalize(ModelView * T).xyz;
    fE = -(ModelView * vPosition).xyz;
    fL =  (View * LightPosition).xyz;

    if( LightPosition.w != 0.0f)
        fL = LightPosition.xyz - vPosition.xyz;

    gl_Position = ( EnableOrtho == 1 ? Orthographic : Projection ) * ModelView * vPosition;
    gl_TexCoord[0].xy = vTexCoords;
}

