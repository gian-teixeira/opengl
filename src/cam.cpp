#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "rotation.hpp"

#define WIDTH 500
#define HEIGHT 500

float zoom = 2.1;
glm::vec4 
    target = { 0.0, 0.0, 0.0, 0.0 },
    rotation = { M_PI/4, 0.0, 0.0, 0.0 };

void draw()
{
    glm::mat4 *rmatrix = get_rotation_matrix(rotation);
    glm::vec4 eye = (target - glm::vec4(0.0, zoom, 0.0, 0.0)) * *rmatrix;
    glm::vec4 up = (target - glm::vec4(0.0, zoom, 1.0, 0.0)) * *rmatrix;

    glPushMatrix();

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gluLookAt(eye.x, eye.y, eye.z,
              target.x, target.y, target.z,
              up.x, up.y, up.z);

    glBegin(GL_QUADS);
        glColor3f(0.0, 0.0, 0.0);
        glVertex3f(-1.0, -1.0, 0.0);
        glVertex3f(1.0, -1.0, 0.0);
        glVertex3f(1.0, 1.0, 0.0);
        glVertex3f(-1.0, 1.0, 0.0);
    glEnd();

    glPopMatrix();
    
    glutSwapBuffers();

    delete rmatrix;
}

void mouse_wheel_CB(int button, int dir, int x, int y)
{
    printf("%f\n", zoom);
    zoom += -0.1 * (float)dir;
    glutPostRedisplay();
}

void resize(GLsizei width, GLsizei height)
{
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (float) width / height, 0.1, 30.0);
    glMatrixMode(GL_MODELVIEW);
}

void init(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Camera");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
}

int main(int argc, char *argv[])
{
    init(&argc, argv);
    glutMouseWheelFunc(mouse_wheel_CB);
    glutReshapeFunc(resize);
    glutDisplayFunc(draw);
    glutMainLoop();

    return 0;
}
