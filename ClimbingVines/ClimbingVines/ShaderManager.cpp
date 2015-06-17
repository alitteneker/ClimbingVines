#include "ShaderManager.h"
#include "MatrixManager.h"

// GLOBALS
const int TEX_UNIT = 0;
const int BUMP_UNIT = 1;
const int GLOSS_UNIT = 2;

// -- Defaults --
vec4 ambient( 0.2f );
vec4 diffuse( 0.6f );
vec4 specular( 0.4f );
float shininess = 100.0f;
vec4 colour = vec4(1, 1, 1, 1);
// -- Defaults --

GLint uModel, uView, uLightPos, uCameraPos, uEyeRelative;
GLint uProjection, uOrthographic, uEnableOrtho, uEnableLight;
GLint uAmbient, uDiffuse, uSpecular, uShininess, uMinDiffuse;
GLint uTex, uBumpMap, uGlossMap, uScaleBump, uTextureAlpha;
GLint uEnableTex, uEnableBump, uEnableGloss;

void initShaderVars(GLuint program) {
    uModel        = glGetUniformLocation( program, "Model"           );
    uProjection   = glGetUniformLocation( program, "Projection"      );
    uOrthographic = glGetUniformLocation( program, "Orthographic"    );
    uEnableOrtho  = glGetUniformLocation( program, "EnableOrtho"     );
    uEnableLight  = glGetUniformLocation( program, "EnableLighting"  );
    uView         = glGetUniformLocation( program, "View"            );
    uAmbient      = glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse      = glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular     = glGetUniformLocation( program, "SpecularProduct" );
    uLightPos     = glGetUniformLocation( program, "LightPosition"   );
    uCameraPos    = glGetUniformLocation( program, "CameraPosition"  );
    uEyeRelative  = glGetUniformLocation( program, "EyeRelative"     );
    uShininess    = glGetUniformLocation( program, "Shininess"       );
    uMinDiffuse   = glGetUniformLocation( program, "MinDiffuse"      );
    uTex          = glGetUniformLocation( program, "Tex"             );
    uEnableTex    = glGetUniformLocation( program, "EnableTex"       );
    uBumpMap      = glGetUniformLocation( program, "NormalMap"       );
    uEnableBump   = glGetUniformLocation( program, "EnableBump"      );
    uGlossMap     = glGetUniformLocation( program, "GlossMap"        );
    uEnableGloss  = glGetUniformLocation( program, "EnableGloss"     );
    uScaleBump    = glGetUniformLocation( program, "ScaleBump"       );
    uTextureAlpha = glGetUniformLocation( program, "TextureAlpha"    );
    
    writeMaterial();
    enableLighting(true);
    set_light_position(30.0f, 30.0f, 30.0f);
    setMinDiffuse(0.05f);
    setTextureAlpha(1.0f);
    setBumpScale(5.0);

    // Set texture sampler variable to texture unit 0
    glUniform1i( uTex,      TEX_UNIT   );
    glUniform1i( uBumpMap,  BUMP_UNIT  );
    glUniform1i( uGlossMap, GLOSS_UNIT );
}
void writeMaterial(Material *m) {
    
    set_ambient(m->k_ambient);
    set_specular(m->k_specular);
    
    if( m->map_Kd != NULL ) {
        enableTex(true);
        bindTexture((GLImage*)m->map_Kd, TEX_UNIT);
    }
    else {
        set_diffuse(m->k_diffuse);
    }
    
    if( m->map_Ks != NULL ) {
        enableGloss(true);
        bindTexture((GLImage*)m->map_Ks, GLOSS_UNIT);
    }
    if( m->map_bump != NULL ) {
        enableBump(true);
        bindTexture((GLImage*)m->map_bump, BUMP_UNIT);
    }
}
void bindTexture(GLImage *texture, GLuint unit) {
    bindTexture(texture->binding, unit);
}
void bindTexture(GLuint texture, GLuint unit) {
    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture( GL_TEXTURE_2D, texture );
}
void writeModel() {
    writeModel(mModel);
}
void writeModel(mat4 model) {
    glUniformMatrix4fv( uModel, 1, GL_TRUE, model );
}
void writeView() {
    writeView(mView);
}
void writeView(mat4 view) {
    glUniformMatrix4fv( uView, 1, GL_TRUE, mView );
}
void writeProjection(mat4 projection) {
    glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
}
void writeOrtho(mat4 ortho) {
    glUniformMatrix4fv( uOrthographic, 1, GL_TRUE, ortho );
}
void enableOrtho(bool enable) {
    glUniform1i( uEnableOrtho, enable ? 1 : 0 );
}
void enableLighting(bool enable) {
    glUniform1i( uEnableLight, enable ? 1 : 0 );
}
void enableTex(bool enable) {
    glUniform1i( uEnableTex, enable ? 1 : 0 );
}
void enableBump(bool enable) {
    glUniform1i( uEnableBump, enable ? 1 : 0 );
}
void enableGloss(bool enable) {
    glUniform1i( uEnableGloss, enable ? 1 : 0 );
}
void setBumpScale(float set) {
    glUniform1f(uScaleBump, set);
}
void setTextureAlpha(float set) {
    glUniform1f(uTextureAlpha, set);
}
void setMinDiffuse(float set) {
    glUniform1f(uMinDiffuse, set);
}
void set_light_position(vec4 set) {
    glUniform4fv(uLightPos, 1, set);
}
void set_light_position(float x, float y, float z) {
    glUniform4fv(uLightPos, 1, vec4(x, y, z, 0));
}
void set_camera_position(vec4 set) {
    glUniform4fv(uCameraPos, 1, set);
}
void enableEyeRelative(bool enable) {
    glUniform1i( uEyeRelative, enable ? 1 : 0 );
}
void set_shininess(float set) {
    shininess = set;
    glUniform1f(uShininess, shininess);
}
void set_ambient(vec4 set) {
    ambient = set;
    glUniform4fv(uAmbient,  1, ambient  * colour);
}
void set_diffuse(vec4 set) {
    diffuse = set;
    glUniform4fv(uDiffuse,  1, diffuse  * colour);
}
void set_specular(vec4 set) {
    specular = set;
    glUniform4fv(uSpecular, 1, specular * colour);
}
void set_colour(float r, float g, float b) {
    colour[0] = r; colour[1] = g; colour[2] = b;
    writeColour();
}
void set_colour(vec4 set) {
    colour = set;
    writeColour();
}
void writeColour() {
    glUniform4fv(uAmbient,  1, ambient  * colour);
    glUniform4fv(uDiffuse,  1, diffuse  * colour);
    glUniform4fv(uSpecular, 1, specular * colour);
}
void writeMaterial() {
    writeColour();
    set_shininess(shininess);
}



// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile) {
    FILE* fp = fopen(shaderFile, "r");
    
    if ( fp == NULL ) { return NULL; }
    
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    
    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);
    
    buf[size] = '\0';
    fclose(fp);
    
    return buf;
}


// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFile, const char* fShaderFile) {
    struct Shader {
        const char*  filename;
        GLenum       type;
        GLchar*      source;
    }  shaders[2] = {
        { vShaderFile, GL_VERTEX_SHADER, NULL },
        { fShaderFile, GL_FRAGMENT_SHADER, NULL }
    };
    
    GLuint program = glCreateProgram();
    
    for ( int i = 0; i < 2; ++i ) {
        Shader& s = shaders[i];
        s.source = readShaderSource( s.filename );
        if ( shaders[i].source == NULL ) {
            std::cerr << "Failed to read " << s.filename << std::endl;
            exit( EXIT_FAILURE );
        }
        
        GLuint shader = glCreateShader( s.type );
        glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
        glCompileShader( shader );
        
        GLint  compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            std::cerr << s.filename << " failed to compile:" << std::endl;
            GLint  logSize;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
            char* logMsg = new char[logSize];
            glGetShaderInfoLog( shader, logSize, NULL, logMsg );
            std::cerr << logMsg << std::endl;
            delete [] logMsg;
            
            exit( EXIT_FAILURE );
        }
        
        delete [] s.source;
        
        glAttachShader( program, shader );
    }
    
    /* link  and error check */
    glLinkProgram(program);
    
    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        std::cerr << "Shader program failed to link" << std::endl;
        GLint  logSize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
        char* logMsg = new char[logSize];
        glGetProgramInfoLog( program, logSize, NULL, logMsg );
        std::cerr << logMsg << std::endl;
        delete [] logMsg;
        
        exit( EXIT_FAILURE );
    }
    
    /* use program object */
    glUseProgram(program);
    
    return program;
}

