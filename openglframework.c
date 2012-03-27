/*
* An OpenGL template/framework file for the Computer Graphics course
* at the University of Groningen.
*/

// If windows is used, windows.h should be included (before gl.h and glu.h)
#if defined(_WIN32)
#include <windows.h>
#endif

// If you use GLUT you do not need to include gl.h and glu.h
// as glut.h guarantees that gl.h and glu.h are properly 
// included.
// So if you do not wish to use  glut.h, uncomment the following lines.
//#include <GL/gl.h>
//#include <GL/glu.h>

#if defined(NEED_GLEW)
#include "glew.h"
#endif

// Apparently Mac OS X puts the GLUT headers somewhere different.
// For windows we use freeglut.
#if defined(__APPLE__)&& defined(__MACH__)
#include <GLUT/glut.h>
#elif defined(_WIN32)
#include <GL/freeglut.h>
#else
#include <GL/glut.h>
#endif

#include "lodepng.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SPHERE_N (20)

typedef struct _Orbit {
    GLfloat center[3];
    GLfloat axis[3];
    GLfloat radius;
    GLfloat speed;
} Orbit;

typedef struct _Planet {
    Orbit orbit;

    /* up-vector and speed for its own rotation */
    GLfloat axis[3];
    GLfloat speed;
    GLfloat radius;
    
    char *name;
    GLuint texture;

    unsigned int num_moons;
    struct _Planet *moons;
} Planet;

enum MouseMode {
    ZOOMING,
    PANNING,
    ROTATING,
    IDLE
};

float camera_x = 0;
float camera_y = 0;

float camera_pitch = 0;
float camera_heading = 0;
float camera_zoom = 0;

int mouse_x = 0;
int mouse_y = 0;

int mouse_dx = 0;
int mouse_dy = 0;

enum MouseMode mouse_mode = IDLE;

GLUquadric *quadric;

int num_planets;
Planet** planets;

void setGlMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat ka, GLfloat kd, GLfloat ks, GLfloat n)
{
    GLfloat ambient[] = {ka*r,ka*g,ka*b,1.0};
    GLfloat diffuse[] = {kd*r,kd*g,kd*b,1.0};
    GLfloat specular[] = {ks,ks,ks,1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, n);
}

void setVector(GLfloat *target, GLfloat x, GLfloat y, GLfloat z)
{
    target[0] = x;
    target[1] = y;
    target[2] = z;
}

GLuint loadTexture(char* filename)
{
   unsigned char* buffer;
   unsigned char* image;
   size_t buffersize, imagesize;
   GLuint texName;
   LodePNG_Decoder decoder;

   LodePNG_loadFile(&buffer, &buffersize, filename);
   LodePNG_Decoder_init(&decoder);
   decoder.infoRaw.color.colorType = 6; /* Load image as RGBA */
   LodePNG_decode(&decoder, &image, &imagesize, buffer, buffersize);
   if(decoder.error) {
      printf("Error reading in png image: %d\n", decoder.error);
      exit(1);      
   } else {
      glPixelStorei(GL_UNPACK_ALIGNMENT,1);
      glGenTextures(1,&texName);
      glBindTexture(GL_TEXTURE_2D,texName);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,decoder.infoPng.width,
                   decoder.infoPng.height,0, GL_RGBA,GL_UNSIGNED_BYTE,image);
   } 
   return texName;
}

void drawPlanet(Planet* planet, float t)
{
    glBindTexture(GL_TEXTURE_2D, planet->texture);

    glPushMatrix();

    // Translate to the center of the orbit
    glTranslatef(planet->orbit.center[0], planet->orbit.center[1], planet->orbit.center[2]);

    // Rotate around the orbit
    glRotatef(t * planet->orbit.speed, planet->orbit.axis[0], planet->orbit.axis[1], planet->orbit.axis[2]);

    // translate to a position in the orbit
    glTranslatef(planet->orbit.radius, 0, 0);

    // Rotate the planet around it's axis
    glRotatef(t * planet->speed, planet->axis[0], planet->axis[1], planet->axis[2]);
    gluSphere(quadric, planet->radius, SPHERE_N, SPHERE_N);
    glPopMatrix();
}

void display(void)
{
    int i; /* C99 is the most ugly thing ever created. It is truly an abomination. */
    float t;

    /* Clear all pixels */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0,0.0,1000.0,0.0,0.0,0.0,0.0,1.0,0.0);

    glPushMatrix();

    // better rotation by rotating around the point 200,200,200
    // glTranslated(200,200,200);

    // zoom & panning
    glTranslatef(camera_x, camera_y, camera_zoom);

    // pitch
    glRotatef(camera_pitch, 1, 0, 0);

    // heading
    glRotatef(camera_heading, 0, 1, 0);

    // ... and back to 0,0,0 origin
    // glTranslated(-200, -200, -200);

    // paint!
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glEnableClientState(GL_VERTEX_ARRAY);

    t = (float) glutGet(GLUT_ELAPSED_TIME) / 20;

    for (i = 0; i < num_planets; ++i)
        drawPlanet(planets[i], t);

    glDisableClientState(GL_VERTEX_ARRAY);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    glutSwapBuffers();
}

// Pressing R resets the camera
void reset_camera()
{
    camera_x = 0;
    camera_y = 0;

    camera_pitch = 0;
    camera_heading = 0;
    camera_zoom = 0;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'r':
        case 'R':
            reset_camera();
            break;
         
        case 'q':
        case 'Q':
        case 27: // ESC key
            printf("Exiting...\n");
            exit(0);
            break;
    }
}

void reshape(int w, int h)
{
    glViewport(0,0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(2.0*atan2(h/2.0,1000.0)*180.0/M_PI,(GLdouble)w/(GLdouble)h,500,5000);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y)
{
    mouse_x = x;
    mouse_y = y;

    mouse_dx = 0;
    mouse_dy = 0;

    if (state == GLUT_UP)
        mouse_mode = IDLE;
    else
        if (glutGetModifiers() & GLUT_ACTIVE_CTRL)
            mouse_mode = ZOOMING;
        else if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
            mouse_mode = PANNING;
        else
            mouse_mode = ROTATING;
}

void motion(int x, int y)
{
    mouse_dx = x - mouse_x;
    mouse_dy = y - mouse_y;
}

void idle()
{
    switch (mouse_mode)
    {
        case ZOOMING:
            camera_zoom += 1.0 / 10000 * mouse_dy;
            break;

        case PANNING:
            camera_x += 1.0 / 10000 * mouse_dx;
            camera_y -= 1.0 / 10000 * mouse_dy;
            break;
        
        case ROTATING:
            camera_heading += 1.0 / 1000 * mouse_dx;
            camera_pitch   += 1.0 / 1000 * mouse_dy;
            break;
        
        default:
        case IDLE:
            break;
    }
    
    glutPostRedisplay();
}

void initLights()
{
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 200.0, 200.0, 200.0 };
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
}

void initQuadric()
{
    quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);
}

void initPlanets()
{
    planets = malloc(sizeof(Planet*) * 2);

    Planet *earth = malloc(sizeof(Planet));
    setVector(earth->orbit.center, 0, 0, 0);
    setVector(earth->orbit.axis, 0, 1, 0);
    earth->orbit.radius = 150; // in million km
    earth->orbit.speed = 1.0 / 10; // should be 1.0 / 365;
    earth->radius = 10;
    earth->texture = loadTexture("textures/earth.png");

    earth->axis[0] = 0;
    earth->axis[1] = 1;
    earth->axis[2] = 0;
    earth->speed = 1;

    planets[0] = earth;
    
    Planet *sun = malloc(sizeof(Planet));
    setVector(sun->orbit.center, 0, 0, 0);
    setVector(sun->orbit.axis, 0, 1, 0);
    sun->orbit.radius = 0;
    sun->radius = 40;
    sun->texture = loadTexture("textures/sun.png");

    setVector(sun->axis, 0, 1, 0);
    sun->speed = 1.0 / 25.38;

    planets[1] = sun;
    num_planets = 2;
}

int main(int argc, char** argv)
{
#if defined(NEED_GLEW)
    GLenum err;
#endif


    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(220,100);
    glutCreateWindow("Computer Graphics - OpenGL framework");

#if defined(NEED_GLEW)
    /* Init GLEW if needed */
    err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(1);
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

    /* Select clearing (background) color */
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    initLights();

    initQuadric();

    initPlanets();

    /* Register GLUT callback functions */
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
