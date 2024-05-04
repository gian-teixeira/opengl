#ifndef __particle_h__
#define __particle_h__

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <ctime>
#include <iostream>

struct particle {
    float rotation_angle;
    float scale;
    float guide;
    glm::vec3 rotation_weights;
    glm::vec3 pos;
    glm::vec3 color;
    int lifespam;
    int step;

    particle(float x, float y, float z, int lifespam) {
        this->rotation_angle = rand()%360;
        this->scale = (double)rand() / RAND_MAX;
        this->rotation_weights = glm::vec3((double)rand()/RAND_MAX, (double)rand()/RAND_MAX, (double)rand()/RAND_MAX);
        this->guide = ((double)rand() - RAND_MAX/2.0) / RAND_MAX;
        this->pos = glm::vec3(x,y,z);
        this->lifespam = lifespam * pow((double)rand()/RAND_MAX, 5);
        this->step = 0;

        float c = (double)rand()/ RAND_MAX;
        this->color = glm::vec3(0.3 + 0.2*(c > 0.4), 0.3 + 0.1*(c <= 0.4), 0.3 + 0.1*(c <= 0.4));
    }

    void draw(bool line = false)
    {
        if(!this->is_active()) return;
        ++this->step;

        glPushMatrix();

        glTranslatef(this->pos.x, this->pos.y, this->pos.z + this->guide);
        glRotatef(this->rotation_angle, this->rotation_weights.x, this->rotation_weights.y, this->rotation_weights.z);
        glScalef(this->scale, this->scale, this->scale);
        glColor4f(this->color.x, this->color.y, this->color.z, sin(M_PI*this->step/this->lifespam));
        glBegin(GL_TRIANGLES);
            glVertex3f(-0.05, -0.05, 0.0);
            glVertex3f(0.05, -0.05, 0.0);
            glVertex3f(0.0, 0.05, 0.0);
        glEnd();
        if(line) {
            glColor4f(0.0, 0.0, 0.0, sin(M_PI*this->step/this->lifespam));
            glBegin(GL_LINE_LOOP);
                glVertex3f(-0.05, -0.05, 0.0);
                glVertex3f(0.05, -0.05, 0.0);
                glVertex3f(0.0, 0.05, 0.0);
            glEnd();
        }

        glPopMatrix();
    }

    inline bool is_active() { return this->step != this->lifespam; }

    bool operator < (particle other) const { return true; }
};

#endif