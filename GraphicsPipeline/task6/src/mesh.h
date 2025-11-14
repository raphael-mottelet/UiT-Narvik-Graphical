#pragma once
#include <glad/glad.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct Mesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei indexCount = 0;
};

// UV-sphere: segmentsX (longitude), segmentsY (latitude), radius
inline Mesh makeSphere(int segmentsX, int segmentsY, float radius){
    segmentsX = std::max(3, segmentsX);
    segmentsY = std::max(2, segmentsY);

    struct V { float px,py,pz; float nx,ny,nz; };
    const int vxCount = (segmentsX + 1) * (segmentsY + 1);

    std::vector<V> verts; verts.reserve(vxCount);
    std::vector<unsigned int> idx; idx.reserve(segmentsX * segmentsY * 6);

    for(int y=0; y<=segmentsY; ++y){
        float v = (float)y / (float)segmentsY;
        float theta = v * glm::pi<float>(); // 0..pi
        float st = std::sin(theta);
        float ct = std::cos(theta);

        for(int x=0; x<=segmentsX; ++x){
            float u = (float)x / (float)segmentsX;
            float phi = u * glm::two_pi<float>(); // 0..2pi
            float sp = std::sin(phi);
            float cp = std::cos(phi);

            // unit sphere
            float nx = cp * st;
            float ny = ct;
            float nz = sp * st;

            V vert;
            vert.px = radius * nx;
            vert.py = radius * ny;
            vert.pz = radius * nz;
            vert.nx = nx;
            vert.ny = ny;
            vert.nz = nz;
            verts.push_back(vert);
        }
    }

    for(int y=0; y<segmentsY; ++y){
        for(int x=0; x<segmentsX; ++x){
            unsigned int i0 =  y    * (segmentsX+1) +  x;
            unsigned int i1 =  i0 + 1;
            unsigned int i2 = (y+1) * (segmentsX+1) +  x;
            unsigned int i3 =  i2 + 1;

            // CCW
            idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
            idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
        }
    }

    Mesh m{};
    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(verts.size()*sizeof(V)), verts.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(idx.size()*sizeof(unsigned int)), idx.data(), GL_STATIC_DRAW);

    // aPos = location 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);

    // aNormal = location 1
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(3*sizeof(float)));

    glBindVertexArray(0);
    m.indexCount = (GLsizei)idx.size();
    return m;
}
