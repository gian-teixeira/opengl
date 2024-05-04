#ifndef __rotation_h__
#define __rotation_h__

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <cmath>

glm::mat4 *get_rotation_matrix(glm::vec4 rotation)
{
    float bx[16] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, cosf(rotation.x), -sinf(rotation.x), 0.0,
        0.0, sinf(rotation.x), cosf(rotation.x), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    
    float by[16] = {
        cosf(rotation.y), 0.0, sin(rotation.y), 0.0,
        0.0, 1.0, 0.0, 0.0,
        -sinf(rotation.y), 0.0, cosf(rotation.y), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    
    float bz[16] = {
        cosf(rotation.z), -sinf(rotation.z), 0.0, 0.0,
        sinf(rotation.z), cosf(rotation.z), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    glm::mat4 rx, ry, rz;
    
    memcpy(glm::value_ptr(rx), bx, sizeof(bx));
    memcpy(glm::value_ptr(ry), by, sizeof(by));
    memcpy(glm::value_ptr(rz), bz, sizeof(bz));

    glm::mat4 *rmatrix = new glm::mat4;
    *rmatrix = rz * ry * rx;
    
    return rmatrix;
}


#endif