#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>

#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <set>

#include "rotation.hpp"
#include "particle.hpp"

#define WIDTH 500
#define HEIGHT 500
#define SPREAD(v) v[0], v[1], v[2]

static GLfloat ambient_light[4] = { 0.5, 0.5, 0.5, 0.0 };
static GLfloat diffuse_light[4] = { 1.0, 0.4, 0.4, 0.0 };
static GLfloat light_position[4] = { 0.0, 1.0, 0.0, 0.0 };
static GLfloat backlight[4] = {0.4, 1.0, 1.0, 0.0};
static GLfloat backlight_pos[4] = {-2.0, -2.0, -2.0, 0.0};

float l = 2.0;
float phi = (1 + sqrt(5)) / 2;
float a = 1 / sqrt(phi * phi + 1);
float b = a * phi;

using node = glm::vec3;
using face = std::array<node,3>;

std::vector<node> vertices = {
    {-a, 0, b}, {a, 0, b}, {-a, 0, -b}, {a, 0, -b},
    {0, b, a}, {0, b, -a}, {0, -b, a}, {0, -b, -a},
    {b, a, 0}, {-b, a, 0}, {b, -a, 0}, {-b, -a, 0}
};

std::vector<std::array<int,3>> polygon = {
    {0, 9, 4}, {0, 4, 1}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
    {8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3},
    {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6}, {0, 1, 6},
    {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}
};

float ring_size = 1.0;
std::vector<node> ringv = { 
    {-ring_size, -ring_size, ring_size}, 
    {-ring_size, ring_size, 0.5}, 
    {0.0, ring_size, 0.0}, 
    {ring_size, ring_size, -0.5}, 
    {ring_size, -ring_size, -ring_size} 
};
std::vector<int> ring = { 4, 3, 2, 1, 0 };

#define FPS 60
#define ANIMATION_INTERVAL 2

int rotation_angle = 0;
unsigned animation_timer = 0;
float shift = 0;
int divide = 1;
int spawn = 0;

std::vector<particle> particles;

void draw_ring()
{
    glLineWidth(6.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP);
    for(float theta = 0.0; theta < 2*M_PI; theta += std::min(M_PI/50,M_PI)) {
        glm::mat3 rz = {
            {cos(theta), -sin(theta), 0},
            {sin(theta), cos(theta), 0},
            {0, 0, 1},
        };    
        glm::vec3 p = rz*glm::vec3(0.8, -0.8, -cos(theta/2));
        if(!spawn) particles.emplace_back(p.x, p.y, p.z, 360);
        //glVertex3f(p.x, p.y, p.z);
        if(theta == 2*M_PI) break;
    }
    spawn = (spawn+1)%30;
    glEnd();
}


void subdivide(const face &f, int i)
{
    if(i == 0) {
        //glm::vec3 normal = glm::normalize(glm::cross(f[1]-f[0],f[2]-f[0]));
        glm::vec3 normal = glm::normalize(glm::cross(f[2]-f[0],f[1]-f[0]));
        glNormal3f(normal.x, normal.y, normal.z);
        for(int i = 0; i < 3; ++i)
            glVertex3f(f[i].x, f[i].y, f[i].z);
        return;
    }

    node med[3] = {f[0]+f[1], f[1]+f[2], f[2]+f[0]};
    for(int i = 0; i < 3; ++i) {
        med[i] = glm::normalize(0.5f*med[i]);
    }

    subdivide({f[0], med[0], med[2]}, i-1);
    subdivide({f[1], med[1], med[0]}, i-1);
    subdivide({f[2], med[2], med[1]}, i-1);
    subdivide({med[0], med[1], med[2]}, i-1);
}

void draw_circle(float x, float y, float z, float r)
{
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x,y,z);
    glColor4f(1.0,0.0,0.0,1.0);
    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0.0, 0.0, 0.0);
        for(float theta = 0.0; theta < 2*M_PI; theta += std::min(M_PI/10,M_PI)) {
            glm::mat3 rz = {
                {cos(theta), -sin(theta), 0},
                {sin(theta), cos(theta), 0},
                {0, 0, 1},
            };    
            glm::vec3 p = rz*glm::vec3(r, 0.0, 0.0);
            glVertex3f(p.x, p.y, p.z);
            printf("%f %f %f\n", p.x, p.y, p.z);
            if(theta == 2*M_PI) break;
        }
        printf("\n");
    glEnd();
    glPopMatrix();
}

void draw_orb(GLenum mode, glm::vec4 color)
{
    glColor4f(color.x, color.y, color.z, color.w);
    glBegin(mode);
        for(std::array<int,3> f : polygon) {
            std::array<node,3> _f = { vertices[f[0]], vertices[f[1]], vertices[f[2]] };
            subdivide(_f,divide);
        }
    glEnd();
}

void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(1000 / FPS, timer, 0);

    if(animation_timer%ANIMATION_INTERVAL == 0) {
        rotation_angle = (rotation_angle + 1) % 360;
    }
    animation_timer = (animation_timer+1)%ANIMATION_INTERVAL;
}

void draw()
{
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gluLookAt(0.0, 3.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    
    glPushMatrix();
    
    GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat mat_emission[] = { 0.5, 0.0, 0.0, 1.0 };
    GLfloat mat_shininess[] = {0.0};

    //glColorMaterial(GL_FRONT, GL_AMBIENT);
    //glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    //glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

    if(shift > 0) {
        glTranslatef(-shift, 0.0, 0.0);
        shift -= shift / 10;
        if(shift < 0) shift = 0;
    }

    draw_ring();

    glTranslatef(0.0, 0.0, 0.2*sin(3 * M_PI*rotation_angle/360.0));
    glRotatef(rotation_angle, 0.0, 0.0, 1.0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    draw_orb(GL_TRIANGLES, glm::vec4(0.4, 0.4, 0.4, 1.0));

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.0);
    draw_orb(GL_TRIANGLES, glm::vec4(0.0, 0.0, 0.0, 1.0));
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    std::vector<particle> mantain;
    int c = particles.size();
    for(int i = 0; i < c; ++i) {
        if(!particles[i].is_active()) continue;
        particles[i].draw(true);
        mantain.push_back(particles[i]);
    }

    /*glNormal3f(-sin(M_PI*rotation_angle/180), cos(M_PI*rotation_angle/180), 0.0);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glVertex3f(-1.0, 0.0, -1.0);
        glVertex3f(-1.0, 0.0, 1.0);
        glVertex3f(1.0, 0.0, 1.0);
        glVertex3f(1.0, 0.0, -1.0);
    glEnd();*/

    glPopMatrix();
    
    glutSwapBuffers();
}

void scroll(int button, int dir, int x, int y)
{
    divide += dir;
    if(divide < 0) divide = 0;
}

int last_width = 0;
int last_height = 0;

void resize(int width, int height)
{
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (float) width / height, 0.1, 30.0);
    glMatrixMode(GL_MODELVIEW);

    shift = (width - last_width) / 100.0;
    last_width = width;
    last_height = height;
}

void init(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_SINGLE| GLUT_RGBA | GLUT_DEPTH); 
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Camera");
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glMatrixMode(GL_PROJECTION);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, backlight);
    glLightfv(GL_LIGHT1, GL_POSITION, backlight_pos);
    
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    init(&argc, argv);
    glutReshapeFunc(resize);
    glutDisplayFunc(draw);
    glutTimerFunc(0,timer,0);
    glutMouseWheelFunc(scroll);
    glutMainLoop();

    

    return 0;
}
