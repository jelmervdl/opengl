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

#define assert_gl_ok _assert_gl_ok(__FILE__, __LINE__);

void _assert_gl_ok(char *file, int line)
{
    if (glGetError() != GL_NO_ERROR)
    {
        printf("OpenGL triggered an error somewhere before %s:%d\n", file, line);
        exit(1);
    }
}

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

GLuint vertices_buffer;
GLuint indices_buffer;
GLuint colors_buffer;

void set_up()
{
    // create buffers
    glGenBuffersARB(1, &vertices_buffer);
    glGenBuffersARB(1, &indices_buffer);
    glGenBuffersARB(1, &colors_buffer);

    // upload vertices
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertices_buffer);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(cubeVertices) / sizeof(GLfloat), cubeVertices, GL_STATIC_DRAW_ARB);

    // upload indices
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indices_buffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(cubeIndices) / sizeof(GLubyte), cubeIndices, GL_STATIC_DRAW_ARB);

    // upload colors
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, colors_buffer);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(cubeColors) / sizeof(GLfloat), cubeColors, GL_STATIC_DRAW_ARB);

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
    gluLookAt(0.0,0.0,5.0,0.0,0.0,0.0,0.0,1.0,0.0);

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

    glutSwapBuffers();

    assert_gl_ok;
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':
        case 'Q':
        case 27: // ESC key
            printf("Exiting...\n");
            tear_down();
            exit(0);
            break;
    }
}

void reshape(int w, int h)
{
    glViewport(0,0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0,(GLdouble)w/(GLdouble)h,1.5,20.0);
    glMatrixMode(GL_MODELVIEW);
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
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    /* Register GLUT callback functions */
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);

    set_up();
    glutMainLoop();
    // tear_down(); // no use, glutMainLoop never ever returns.

    return 0;
}
