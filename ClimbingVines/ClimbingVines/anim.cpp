////////////////////////////////////////////////////
// anim.cpp version 4.1
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

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
#include <stdlib.h>
#include <math.h>

#include "Timer.h"
#include "matm.h"
#include "Primitive.h"
#include "Primitives.h"
#include "PlantPrimitives.h"
#include "ShapeData.h"
#include "PrimitiveShape.h"
#include "Tree.h"
#include "GLOBJParser.h"

#include "MatrixManager.h"
#include "TextureLoader.h"
#include "ShaderManager.h"
#include "CameraManager.h"
#include "KeyboardManager.h"

#ifdef __APPLE__
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#endif

int Width  = 800;
int Height = 800;
int Button = -1;
int PrevY  = 0;
int PrevX  = 0;

Timer TM;
double TIME = 0.0;
bool Animate = true;

int framecount;
double timecount;
bool msaa = true;
bool bumpEn = true;
bool glossEn = true;
bool orthEn = false;

void instructions();
#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2

// The Tree
Tree* tree;

// store data for the environment
PrimitiveShape* building;

// Sky box (sphere) texture
GLImage* sky_texture;
GLImage *ground_texture, *ground_gloss, *ground_normal;
ShapeData* ground_box;

/*********************************************************
**********************************************************/
void myinit(void) {
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "../vshader.glsl", "../fshader.glsl" );
    glUseProgram(program);

    // Generate vertex arrays for geometric shapes, and ensure all relevant shader variables are set
    initPrimitives(program);
    initShaderVars(program);

    // load the sky texture, and set a background color (which we won't see)
    glClearColor( 0.51, 0.792, 1.0, 1.0 ); // sky blue background
    sky_texture = loadTexture("sky_texture3.tga");
    
    // load some textures for the ground plane, and generate a custom cube with repeating textures on it
    ground_texture = loadTexture("ground_texture.tga");
    ground_normal = loadTexture("ground_normal.tga");
    ground_gloss = loadTexture("ground_grey.tga");
    ground_box = genCube(program, 50);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Enable multisampling (if the flag is set)
    if( msaa )
        glEnable(GL_MULTISAMPLE_ARB);
    
    // Create our tree!
//    tree = new Tree(program);
    
    // load the house
    vector<Primitive*> raw = GL_parse_obj_file("house_obj.obj");
    printf("Loaded %lu primitives.\n", raw.size());
    building = new PrimitiveShape(raw[0]->material);
    for( int i = 0; i < raw.size(); ++i )
        building->addPrimitive(raw[i]);
    building->buildShape(program);
}



/*********************************************************
**********************************************************/

// These are some (slightly annoying) variables for camera control
float new_dist = 0;
float eye_dist = 4;
float ref_height = 0;
float eye_height = 1;
float time_since_change = 0;
int prev_segments = 0;
bool go_out = false;
float circle_time_scale = 0.5f;
float rotation = 0;

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // make sure our matrices are cleared, and ready for real work
    mModel.setIdentity(); mView.setIdentity();
    camera.setVars();
    
    // update the time counters
    ++framecount;
    timecount += TM.GetElapsedTime();
    TM.Reset();
    
    // Set up all the right flags for general rendering
    enableOrtho(orthEn);
    enableEyeRelative(false);
    enableLighting(true);

//    // Draw the tree!
//    mstack.push(mModel);
//    tree->draw();
//    mModel = mstack.pop();
    
    // draw the building
    mstack.push(mModel);
    mModel *= Translate(0,1,0) * Scale(0.01f);
    building->draw();
    mModel = mstack.pop();
    
    // draw a ground plane
    enableTex(true);
    enableBump(true);
    enableGloss(true);
    bindTexture(ground_texture, TEX_UNIT);
    bindTexture(ground_normal, BUMP_UNIT);
    bindTexture(ground_gloss, GLOSS_UNIT);
    set_colour(1,1,1);
    setTextureAlpha(1.0);
    setBumpScale(5.0);
    mstack.push(mModel);
    mModel *= Translate(0, -0.5, 0) * Scale(1000,1,1000);
    ground_box->draw();
    mModel = mstack.pop();
    
    // Draw the sky sphere (note the lighting disable, and the eye-relative positioning)
    mstack.push(mModel);
    enableTex(true);
    enableGloss(false);
    enableBump(false);
    enableLighting(false);
    enableEyeRelative(true);
    bindTexture(sky_texture, TEX_UNIT);
    mModel *= Scale(500) * RotateY(180);
    drawSphere();
    mModel = mstack.pop();
    
    // just for debugging, draw a sphere around the light position
//    mstack.push(mModel);
//    mModel *= Translate(30, 30, 30);
//    drawSphere();
//    mModel = mstack.pop();

    // just to be safe, empty out the matrix stack, and swap buffers
    mstack.empty();
    glutSwapBuffers();
}

/**********************************************
    PROC: myReshape()
    DOES: handles the window being resized 
    
      -- don't change
**********************************************************/
void myReshape(int w, int h) {
    Width = w;
    Height = h;

    glViewport(0, 0, w, h);

    float orthScale = 0.01;
    mat4 projection = Perspective(50.0f, (float)w/(float)h, 0.1f, 1000.0f);
    mat4 ortho = Ortho(-orthScale*Width/2.0f, orthScale*Width/2.0f, -orthScale*Height/2.0f, orthScale*Height/2.0f, 1.0f, 1000.0f);
    writeProjection(projection);
    writeOrtho(ortho);
}

void instructions()  {
    printf("Press:\n");
    printf("  arrow keys (or drag 1st mouse button) to rotate camera about center.\n");
    printf("  -= (or drag 2nd mouse button) to change distance from camera to center.\n");
    printf("  [] to roll camera.\n");
    printf("  r to restore the original view.\n");
    printf("  0 to set it to the zero state.\n");
    printf("  space to toggle the animation.\n");
    printf("  / to toggle multisampling.\n");
    printf("  q to quit.\n");
}

// start or end interaction
void myMouseCB(int button, int state, int x, int y) {
    Button = button ;
    PrevX = x;
    PrevY = y;

    // Tell the system to redraw the window
    glutPostRedisplay() ;
}

// interaction (mouse motion)
void myMotionCB(int x, int y) {
    if( Button == GLUT_LEFT_BUTTON ) {
        float horiz =  0.5 * (x-PrevX);
        float vert  = -0.5 * (y-PrevY);
        camera.rotateAroundHorizontal(horiz);
        camera.rotateAroundVertial(vert);

        glutPostRedisplay();
    }
    else if( Button == GLUT_RIGHT_BUTTON ) {
        float dist = 0.1f * (y-PrevY);
        camera.changeAroundDistance(dist);
        glutPostRedisplay();
    }
    PrevX = x;
    PrevY = y;
}

void keyPressed(unsigned char key) {
    switch (key) {
        case 'q':
        case 27:
            exit(0);
        case 'r':
            camera.reset();
            glutPostRedisplay();
            break ;
        case '/':
            msaa = !msaa;
            if( msaa )
                glEnable(GL_MULTISAMPLE_ARB);
            else
                glDisable(GL_MULTISAMPLE_ARB);
            glutPostRedisplay();
            break;
        case ' ': // togle animation
            Animate = !Animate ;
            TM.Reset() ;
            break ;
        case '0':
            //reset your object
            break ;
        case 'h':
            bumpEn = !bumpEn;
            glutPostRedisplay();
            break;
        case 'g':
            glossEn = !glossEn;
            glutPostRedisplay();
            break;
        case 'o':
            orthEn = !orthEn;
            glutPostRedisplay();
            break;
        case '?':
            instructions();
            break;
    }
}

void idleCB(void) {
    if( Animate ) {
        // Do some basic FPS calculations
        if( timecount > 1 ) {
            printf("Time %3.0f FPS: %f \n", TIME, framecount/timecount);
            framecount = 0;
            timecount = 0;
        }

//        tree->passTime(TIME);

        glutPostRedisplay();
    }
    if( keyManager.handleKeys() )
        glutPostRedisplay();
}
/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    // Adding GLUT_3_2_CORE_PROFILE to the following will use newer openGL versions, but also completely break shaders
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE );
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(Width, Height);
    glutCreateWindow(argv[0]);
    printf("GL version %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();
    
    myinit();

    keyManager.init();
    keyManager.addKeyPressedListener(keyPressed);

    glutIdleFunc(idleCB) ;
    glutReshapeFunc(myReshape);
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
//    instructions();

    glutDisplayFunc(display);
    
    TM.Reset();
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}




