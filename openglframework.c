/*
* An OpenGL template/framework file for the Computer Graphics course
* at the University of Groningen.
*/

#define _USE_MATH_DEFINES

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

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define assert_gl_ok _assert_gl_ok(__FILE__, __LINE__);

void _assert_gl_ok(char *file, int line)
{
    if (glGetError() != GL_NO_ERROR)
    {
        printf("OpenGL triggered an error somewhere before %s:%d\n", file, line);
        exit(1);
    }
}

typedef enum {
    ZOOMING,
    PANNING,
    ROTATING,
    IDLE
} MouseMode;

typedef struct {
    float x;
    float y;
    float z;  
} Point;

/*
  3 ------ 2
 /|       /|
7 ------ 6 |
| |      | |
| 0 -----| 1
|/       |/
4 ------ 5
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

GLuint vertices_buffer;
GLuint indices_buffer;
GLuint colors_buffer;

Point camera = {0, 0, 5};

float camera_pitch = 0;
float camera_heading = 0;

int mouse_x = 0;
int mouse_y = 0;

int mouse_dx = 0;
int mouse_dy = 0;

MouseMode mouse_mode = IDLE;

Point reference_point(Point eye)
{
    Point center;
    float cc = cos(camera_pitch * M_PI / 180);

    center.x = eye.x + cos(camera_heading * M_PI / 180) * cc;
    center.y = eye.y + -sin(camera_heading * M_PI / 180) * cc;
    center.z = eye.z + sin(camera_pitch * M_PI / 180);

    return center;
}

void print_point(Point p)
{
    printf("Point [x:%f y:%f z:%f, pitch:%f heading:%f]\n", p.x, p.y, p.z, camera_pitch, camera_heading);
}

float rad(float deg)
{
    return deg / 180.0 * M_PI;
}

void gl_unbind_buffers()
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void set_up()
{
    // create buffers
    glGenBuffersARB(1, &vertices_buffer);
    glGenBuffersARB(1, &indices_buffer);
    glGenBuffersARB(1, &colors_buffer);

    // upload vertices
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertices_buffer);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW_ARB);

    // upload indices
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indices_buffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW_ARB);

    // upload colors
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, colors_buffer);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW_ARB);

    assert_gl_ok
}

void tear_down()
{
    glDeleteBuffersARB(1, &vertices_buffer);
    glDeleteBuffersARB(1, &indices_buffer);
    glDeleteBuffersARB(1, &colors_buffer);
}

void display(void)
{
    /* Clear all pixels */
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0f,0.0f,1.0f);
    glLoadIdentity();

    gluLookAt(
        0.0,0.0,0.0,
        0.0,0.0,-1.0,
        0.0,1.0,0.0);

    glPushMatrix();

    // pitch
    glRotatef(camera_pitch, 1, 0, 0);

    // heading
    glRotatef(camera_heading, 0, 1, 0);

    // move camera to the center of the scene
    glTranslatef(-camera.x, -camera.y, -camera.z);

    // .. and drawing!
    glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

    // let GL know about our colors
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, colors_buffer);
    glColorPointer(3, GL_FLOAT, 0, 0);
    
    // point to the vertices
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertices_buffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);

    // draw a cube
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indices_buffer);
	glDrawElements(GL_QUADS, sizeof(cubeIndices) / sizeof(GLubyte), GL_UNSIGNED_BYTE, 0);

    // deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glPopMatrix();

    glutSwapBuffers();

    assert_gl_ok;
}

// Pressing R resets the camera
void reset_camera()
{
    camera.x = 0;
    camera.y = 0;
    camera.z = 5;

    camera_pitch = 0;
    camera_heading = 0;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'r':
        case 'R':
            reset_camera();
            break;
        
        case 'w':
            camera.x += asin(rad(camera_heading));
            camera.y += asin(rad(camera_pitch));
            camera.z -= acos(rad(camera_heading));
            break;
        
        case 's':
            camera.x -= asin(rad(camera_heading));
            camera.y -= asin(rad(camera_pitch));
            camera.z += acos(rad(camera_heading));
            break;  
        
        case 'a':
            camera.x -= 1.0;
            break;
        
        case 'd':
            camera.x += 1.0;
            break;

        case 'q':
        case 'Q':
        case 27: // ESC key
            printf("Exiting...\n");
            tear_down();
            exit(0);
            break;
    }

    print_point(camera);
}

void reshape(int w, int h)
{
    glViewport(0,0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0,(GLdouble)w/(GLdouble)h,1.5,20.0);
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

float limit(float n, float low, float high)
{
    if (n < low)
        return low;
    else if (n > high)
        return high;
    else
        return n;
}

void idle()
{
    switch (mouse_mode)
    {
        case ZOOMING:
            camera.z += 1.0 / 10000 * mouse_dy;
            break;

        case PANNING:
            camera.x += 1.0 / 10000 * mouse_dx;
            camera.y -= 1.0 / 10000 * mouse_dy;
            break;
        
        case ROTATING:
            camera_heading += fmod(1.0 / 1000 * mouse_dx, 360);
            camera_pitch   += limit(1.0 / 1000 * mouse_dy, -90, 90);
            break;
        
        default:
        case IDLE:
            break;
    }
    
    glutPostRedisplay();
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
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    /* Register GLUT callback functions */
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(idle);

    set_up();

    glutMainLoop();
    // tear_down(); // no use, glutMainLoop never ever returns.

    return 0;
}
