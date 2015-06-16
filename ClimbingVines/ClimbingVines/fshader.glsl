#version 120

uniform vec4  LightPosition;

uniform sampler2D Tex, NormalMap, GlossMap;
uniform int EnableTex, EnableBump, EnableGloss;

uniform int EnableLighting;
uniform float Shininess, MinDiffuse, ScaleBump, TextureAlpha;
uniform vec4  AmbientProduct, DiffuseProduct, SpecularProduct;

// per-fragment interpolated values from the vertex shader
varying vec3 fN, fT, fL, fE;

void main() {

    // Normalize the input lighting vectors
    vec3 N = normalize(fN);
    vec3 T = (length(fT) > 0) ? normalize(fT) : fT;
    vec3 E = normalize(fE);
    vec3 L = normalize(fL);
    
    vec2 texcoord = vec2(gl_TexCoord[0]);
    
    vec4 colour = vec4(1.0, 1.0, 1.0, 1.0);
    
    // Load the texture color, textures are enabled
    if( EnableTex == 1 )
        colour = ( colour * ( 1 - TextureAlpha ) ) + ( TextureAlpha * texture2D(Tex, texcoord) );
    
    if( EnableLighting == 1 ) {
        // Set a new normal, scaled as set, using the normal map
        if( EnableBump == 1 && length(T) > 0 ) {
            // Could transform to TBN space in vertex shader, but that produces bad interpolation for curved surfaces
            vec3 B = cross( T, N );
            mat3 TBN = mat3( T, B, N );
            vec3 bumpNorm = 2.0 * texture2D(NormalMap, texcoord).rgb - 1.0;
            bumpNorm.xy *= ScaleBump;
            N = normalize( TBN * bumpNorm );
        }

        // Ambient
        vec4 ambient = AmbientProduct;

        // Diffuse
        float Kd = max(dot(L, N), MinDiffuse);
        vec4 diffuse = Kd * DiffuseProduct * colour;

        // Specular (Blinn-Torrance)
        vec3 H = normalize( L + E );
        // discard the specular highlight if the light's behind the vertex
        float Ks = ( dot(L, N) < 0.0 ) ? 0.0 : pow(max(dot(N, H), 0.0), Shininess);
        // load a gloss value, if the gloss map is enabled
        float gloss = ( EnableGloss == 1 ) ? texture2D(GlossMap, texcoord).r : 1.0;
        vec4 specular = Ks * gloss * SpecularProduct;
        
        colour = vec4( (ambient + diffuse + specular).rgb, 1.0);
    }

    gl_FragColor = colour;
} 

