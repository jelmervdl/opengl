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



#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SPHERE_N (20)

// GLfloat cubeVertices[8*3] = {-1,-1,-1, -1,-1, 1, -1, 1,-1,  1,-1,-1, -1, 1, 1,  1,-1, 1,  1, 1,-1,  1, 1, 1};
// GLubyte cubeIndices[2*12] = {
//         0,1, 0,2, 0,3,                /* From three minusses to two minusses */
//         1,4, 1,5, 2,4, 2,6, 3,5, 3,6, /* From two minusses to one minus */
//         4,7, 5,7, 6,7                 /* From one minus to zero minusses */
//     };

/*
  3 ----- 2
 /|      /|
7 ----- 6 |
| |     | |
| 0 ----| 1
|/      |/
4 ----- 5
*/

GLfloat cubeVertices[] = {
    -1,-1,-1, // 0
     1,-1,-1, // 1
     1, 1,-1, // 2
    -1, 1,-1, // 3
    -1,-1, 1, // 4
     1,-1, 1, // 5
     1, 1, 1, // 6
    -1, 1, 1, // 7
};

GLubyte cubeIndices[] = {
    0,1,2,3, // back
    0,1,5,4, // bottom
    4,7,3,0, // left
    1,2,6,5, // right
    2,3,7,6, // top
    4,5,6,7  // front
};

GLfloat cubeColors[] = {
    1,0,0, // 0 - left - red
    1,1,1, // 1
    1,1,1, // 2
    1,1,0, // 3 - back - yellow
    0,1,0, // 4 - bottom - lime
    0,0,1, // 5 - right - blue
    0,1,1, // 6 - top - cyan
    1,0,1  // 7 - front - magenta
};

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

void display(void)
{
    /* Clear all pixels */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(200.0,200.0,1000.0,200.0,200.0,0.0,0.0,1.0,0.0);

    glPushMatrix();

    // better rotation by rotating around the point 200,200,200
    glTranslated(200,200,200);

    // zoom & panning
    glTranslatef(camera_x, camera_y, camera_zoom);

    // pitch
    glRotatef(camera_pitch, 1, 0, 0);

    // heading
    glRotatef(camera_heading, 0, 1, 0);

    // ... and back to 0,0,0 origin
    glTranslated(-200, -200, -200);

    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    setGlMaterial(0.0f,0.0f,1.0f,0.2,0.7,0.5,64);
    glPushMatrix();
    glTranslated(90,320,100);
    glutSolidSphere(50,SPHERE_N,SPHERE_N);
    glPopMatrix();

    setGlMaterial(0.0f,1.0f,0.0f,0.2,0.3,0.5,8);
    glPushMatrix();
    glTranslated(210,270,300);
    glutSolidSphere(50,SPHERE_N,SPHERE_N);
    glPopMatrix();

    setGlMaterial(1.0f,0.0f,0.0f,0.2,0.7,0.8,32);
    glPushMatrix();
    glTranslated(290,170,150);
    glutSolidSphere(50,SPHERE_N,SPHERE_N);
    glPopMatrix();

    setGlMaterial(1.0f,0.8f,0.0f,0.2,0.8,0.0,1);
    glPushMatrix();
    glTranslated(140,220,400);
    glutSolidSphere(50,SPHERE_N,SPHERE_N);
    glPopMatrix();

    setGlMaterial(1.0f,0.5f,0.0f,0.2,0.8,0.5,32);
    glPushMatrix();
    glTranslated(110,130,200);
    glutSolidSphere(50,SPHERE_N,SPHERE_N);
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

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
    gluPerspective(2.0*atan2(h/2.0,1000.0)*180.0/M_PI,(GLdouble)w/(GLdouble)h,500,1000);
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

void init_light()
{
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { -200.0, 600.0, 1500.0 };
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
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
    glClearColor(0.0,0.0,0.0,0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    init_light();

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
