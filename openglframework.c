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
    GLfloat angle;
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
    struct _Planet **moons;

    GLfloat mat_emission[4];
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

float speed = 1;

enum MouseMode mouse_mode = IDLE;

GLUquadric *quadric;

Planet *sun;

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

void setColor(GLfloat *target, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    target[0] = r;
    target[1] = g;
    target[2] = b;
    target[3] = a;
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

void drawCircle(float radius)
{
    float n;

    glBegin(GL_LINE_LOOP);

    // Disable all the other material properties completely
    // setGlMaterial(0.0, 0.0, 0.0, 0, 0, 0, 0);

    for (n = 0; n < 1.0; n += 0.01)
    {
        glColor3f(1.0 - n, 0, 0);
        glVertex3f(cos(n * 2 * M_PI) * radius, 0, sin(n * 2 * M_PI) * radius);
    }

    // GLfloat no_emission[] = {0, 0, 0, 1};
    // glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);

    glEnd();
}

void drawPlanet(Planet* planet, float t)
{
    int i;

    glPushMatrix();

    // Translate to the center of the orbit
    glTranslatef(planet->orbit.center[0], planet->orbit.center[1], planet->orbit.center[2]);

    // Rotate into orbit
    glRotatef(planet->orbit.angle, planet->orbit.axis[0], planet->orbit.axis[1], planet->orbit.axis[2]);

    // Rotate the planet around its orbit
    glRotatef(fmod(t * planet->orbit.speed, 360), 0, 1, 0);

    drawCircle(planet->orbit.radius);

    // translate to a position in the orbit
    glTranslatef(planet->orbit.radius, 0, 0);

    for (i = 0; i < planet->num_moons; ++i)
        drawPlanet(planet->moons[i], t);

    // Rotate the planet around it's axis
    glRotatef(fmod(t * planet->speed, 360), planet->axis[0], planet->axis[1], planet->axis[2]);

    // Draw the planet
    glBindTexture(GL_TEXTURE_2D, planet->texture);

    // 
    glColor4fv(planet->mat_emission);
    gluSphere(quadric, planet->radius, SPHERE_N, SPHERE_N);

    // disable the texture again
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

void display(void)
{
    // C99 is the most ugly thing ever created. It is truly an abomination.
    float t;

    /* Clear all pixels */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0,0.0,1000.0, 0.0,0.0,0.0, 0.0,1.0,0.0);
    
    glPushMatrix();

    // zoom & panning
    glTranslatef(camera_x, camera_y, camera_zoom);

    // pitch
    glRotatef(camera_pitch, 1, 0, 0);

    // heading
    glRotatef(camera_heading, 0, 1, 0);

    // paint!
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    // glEnableClientState(GL_COLOR_ARRAY);

    t = (float) glutGet(GLUT_ELAPSED_TIME) / 20;

    drawPlanet(sun, t * speed);

    // glDisableClientState(GL_COLOR_ARRAY);
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
        
        case '+':
        case '=':
            speed++;
            break;

        case '-':
            speed--;
            break;

        case '0':
            speed = 1;
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
    gluPerspective(2.0*atan2(h/2.0,1000.0)*180.0/M_PI,(GLdouble)w/(GLdouble)h,1,5000);
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
            camera_zoom += 1.0 / 1000 * mouse_dy;
            break;

        case PANNING:
            camera_x += 1.0 / 1000 * mouse_dx;
            camera_y -= 1.0 / 1000 * mouse_dy;
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
    GLfloat light_ambient[] = {0, 0, 0, 1};
    GLfloat light_diffuse[] = {1, 1, 1, 1};
    GLfloat light_specular[] = {1, 1, 1, 1};
    GLfloat light_shininess[] = { 50.0 };
    GLfloat light_position[] = { -100.0, 0.0, 0.0 };
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_SHININESS, light_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
}

void initQuadric()
{
    quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);
}

Planet *initPlanet()
{
    Planet *planet = malloc(sizeof(Planet));
    
    setVector(planet->orbit.center, 0, 0, 0);
    setVector(planet->orbit.axis, 0, 1, 0);
    planet->orbit.angle = 0;
    planet->orbit.speed = 0;

    setVector(planet->axis, 0, 1, 0);
    planet->speed = 1;

    planet->num_moons = 0;

    planet->texture = 0;
    setColor(planet->mat_emission, 0, 0, 0, 1);

    return planet;
}

void addMoon(Planet *moon, Planet *planet)
{
    // Twilight..
    Planet** new_moons = malloc(sizeof(Planet*) * planet->num_moons + 1);

    // Copy all of its current moon (pointers)
    memcpy(new_moons, planet->moons, sizeof(Planet*) * planet->num_moons);

    new_moons[planet->num_moons] = moon;

    // Free old list of planets (if there was one)
    if (planet->num_moons > 0)
        free(planet->moons);

    planet->moons = new_moons;
    planet->num_moons++;
}

void initPlanets()
{
    sun = initPlanet();
    sun->orbit.radius = 0;
    sun->radius = 45;
    sun->texture = loadTexture("textures/sun.png");
    sun->speed = 1.0 / 25.38;
    setColor(sun->mat_emission, .3, .3, .3, 1.0);
    
    Planet *mercury = initPlanet();
    mercury->orbit.radius = 60; // in million km
    mercury->orbit.speed = 1.0 / 6; // should be 1.0 / 365;
    mercury->radius = 5;
    mercury->texture = loadTexture("textures/mercury.png");
    mercury->speed = 1;
    
    addMoon(mercury, sun);
    
    Planet *venus = initPlanet();
    venus->orbit.radius = 90; // in million km
    venus->orbit.speed = 1.0 / 14; // should be 1.0 / 365;
    venus->radius = 10;
    venus->texture = loadTexture("textures/venus.png");
    venus->speed = 1;
    
    addMoon(venus, sun);

    Planet *earth = initPlanet();
    earth->orbit.radius = 150; // in million km
    earth->orbit.speed = 1.0 / 20; // should be 1.0 / 365;
    earth->radius = 10;
    earth->texture = loadTexture("textures/earth.png");
    earth->speed = 1;

    addMoon(earth, sun);

    Planet *moon = initPlanet();
    setVector(moon->orbit.axis, 1, .5, 0);
    moon->orbit.angle = 35;
    moon->orbit.radius = 20;
    moon->orbit.speed = 1.0 / 2; // 1.0 / 28;
    moon->radius = 5;
    moon->texture = loadTexture("textures/moon.png");
    moon->speed = 1.0 / 5;
    
    addMoon(moon, earth);
    
    Planet *mars = initPlanet();
    mars->orbit.radius = 250; // in million km
    mars->orbit.speed = 1.0 / 24; // should be 1.0 / 365;
    mars->radius = 9;
    mars->texture = loadTexture("textures/mars.png");
    mars->speed = 1;
    
    addMoon(mars, sun);
    
    Planet *jupiter = initPlanet();
    jupiter->orbit.radius = 350; // in million km
    jupiter->orbit.speed = 1.0 / 30; // should be 1.0 / 365;
    jupiter->radius = 25;
    jupiter->texture = loadTexture("textures/jupiter.png");
    jupiter->speed = 0.8;
    
    addMoon(jupiter, sun);
    
    Planet *saturn = initPlanet();
    setVector(saturn->orbit.axis, .15, .8, 0);
    saturn->orbit.angle = 20;
    saturn->orbit.radius = 450; // in million km
    saturn->orbit.speed = 1.0 / 28; // should be 1.0 / 365;
    saturn->radius = 20;
    saturn->texture = loadTexture("textures/saturn.png");
    saturn->speed = 0.7;
    
    addMoon(saturn, sun);
    
    Planet *uranus = initPlanet();
    uranus->orbit.radius = 550; // in million km
    uranus->orbit.speed = 1.0 / 35; // should be 1.0 / 365;
    uranus->radius = 15;
    uranus->texture = loadTexture("textures/uranus.png");
    uranus->speed = 0.9;
    
    addMoon(uranus, sun);
    
    Planet *neptune = initPlanet();
    neptune->orbit.radius = 650; // in million km
    neptune->orbit.speed = 1.0 / 28; // should be 1.0 / 365;
    neptune->radius = 15;
    neptune->texture = loadTexture("textures/neptune.png");
    neptune->speed = 0.9;
    
    addMoon(neptune, sun);
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

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_EMISSION);

    initLights();

    setGlMaterial(1.0, 1.0, 1.0, .1, .9, .3, 16);

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
