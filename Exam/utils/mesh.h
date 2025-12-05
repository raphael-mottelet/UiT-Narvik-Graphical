#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

// Mesh descriptor with OpenGL object handles and index count
struct Mesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei indexCount = 0;
};

// UV sphere mesh generator; segmentsX = longitude, segmentsY = latitude, radius in world units
inline Mesh makeSphere(int segmentsX, int segmentsY, float radius){
    segmentsX = std::max(3, segmentsX);  // Minimum horizontal tessellation
    segmentsY = std::max(2, segmentsY);  // Minimum vertical tessellation

    struct V { float px,py,pz; float nx,ny,nz; };  // Packed vertex format

    const int vxCount = (segmentsX + 1) * (segmentsY + 1);  // Total vertex count for sphere grid

    std::vector<V> verts;            // Vertex buffer container
    verts.reserve(vxCount);
    std::vector<unsigned int> idx;   // Index buffer container
    idx.reserve(segmentsX * segmentsY * 6);

    // Latitude loop from south pole to north pole
    for(int y = 0; y <= segmentsY; ++y){
        float v = (float)y / (float)segmentsY;      // Normalized latitude coordinate
        float theta = v * glm::pi<float>();         // Polar angle in radians
        float st = std::sin(theta);
        float ct = std::cos(theta);

        // Longitude loop around full 360 degrees
        for(int x = 0; x <= segmentsX; ++x){
            float u = (float)x / (float)segmentsX;  // Normalized longitude coordinate
            float phi = u * glm::two_pi<float>();   // Azimuth angle in radians
            float sp = std::sin(phi);
            float cp = std::cos(phi);

            float nx = cp * st;                     // Unit normal X
            float ny = ct;                          // Unit normal Y
            float nz = sp * st;                     // Unit normal Z

            V vert;
            vert.px = radius * nx;                  // Position X on sphere surface
            vert.py = radius * ny;                  // Position Y on sphere surface
            vert.pz = radius * nz;                  // Position Z on sphere surface
            vert.nx = nx;                           // Normal X
            vert.ny = ny;                           // Normal Y
            vert.nz = nz;                           // Normal Z
            verts.push_back(vert);
        }
    }

    // Triangle indices for sphere grid quads
    for(int y = 0; y < segmentsY; ++y){
        for(int x = 0; x < segmentsX; ++x){
            unsigned int i0 =  y    * (segmentsX + 1) +  x;
            unsigned int i1 =  i0 + 1;
            unsigned int i2 = (y+1) * (segmentsX + 1) +  x;
            unsigned int i3 =  i2 + 1;

            idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
            idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
        }
    }

    Mesh m{};
    glGenVertexArrays(1, &m.vao);                             // VAO allocation
    glBindVertexArray(m.vao);                                 // VAO binding

    glGenBuffers(1, &m.vbo);                                  // VBO allocation
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);                     // VBO binding
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(verts.size() * sizeof(V)),
                 verts.data(),
                 GL_STATIC_DRAW);                             // Vertex data upload

    glGenBuffers(1, &m.ebo);                                  // EBO allocation
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);             // EBO binding
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(idx.size() * sizeof(unsigned int)),
                 idx.data(),
                 GL_STATIC_DRAW);                             // Index data upload

    // Vertex attribute layout: location 0 = position, location 1 = normal
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);                                     // VAO unbind
    m.indexCount = (GLsizei)idx.size();                       // Index count assignment
    return m;
}

// Unit cube mesh generator centered at origin with side length 1
inline Mesh makeBox(){
    struct V { float px,py,pz; float nx,ny,nz; };             // Packed vertex format

    // Positions and normals for six faces, four vertices per face
    const V verts[] = {
        // +X
        { 0.5f,-0.5f,-0.5f,  1,0,0 },
        { 0.5f, 0.5f,-0.5f,  1,0,0 },
        { 0.5f, 0.5f, 0.5f,  1,0,0 },
        { 0.5f,-0.5f, 0.5f,  1,0,0 },
        // -X
        {-0.5f,-0.5f, 0.5f, -1,0,0 },
        {-0.5f, 0.5f, 0.5f, -1,0,0 },
        {-0.5f, 0.5f,-0.5f, -1,0,0 },
        {-0.5f,-0.5f,-0.5f, -1,0,0 },
        // +Y
        {-0.5f, 0.5f,-0.5f,  0,1,0 },
        {-0.5f, 0.5f, 0.5f,  0,1,0 },
        { 0.5f, 0.5f, 0.5f,  0,1,0 },
        { 0.5f, 0.5f,-0.5f,  0,1,0 },
        // -Y
        {-0.5f,-0.5f, 0.5f,  0,-1,0 },
        {-0.5f,-0.5f,-0.5f,  0,-1,0 },
        { 0.5f,-0.5f,-0.5f,  0,-1,0 },
        { 0.5f,-0.5f, 0.5f,  0,-1,0 },
        // +Z
        {-0.5f,-0.5f, 0.5f,  0,0,1 },
        { 0.5f,-0.5f, 0.5f,  0,0,1 },
        { 0.5f, 0.5f, 0.5f,  0,0,1 },
        {-0.5f, 0.5f, 0.5f,  0,0,1 },
        // -Z
        { 0.5f,-0.5f,-0.5f,  0,0,-1 },
        {-0.5f,-0.5f,-0.5f,  0,0,-1 },
        {-0.5f, 0.5f,-0.5f,  0,0,-1 },
        { 0.5f, 0.5f,-0.5f,  0,0,-1 }
    };

    // Triangle indices for each cube face
    const unsigned int idx[] = {
        // +X
        0,1,2, 0,2,3,
        // -X
        4,5,6, 4,6,7,
        // +Y
        8,9,10, 8,10,11,
        // -Y
        12,13,14, 12,14,15,
        // +Z
        16,17,18, 16,18,19,
        // -Z
        20,21,22, 20,22,23
    };

    Mesh m{};
    glGenVertexArrays(1, &m.vao);                             // VAO allocation
    glBindVertexArray(m.vao);                                 // VAO binding

    glGenBuffers(1, &m.vbo);                                  // VBO allocation
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);                     // VBO binding
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Vertex data upload

    glGenBuffers(1, &m.ebo);                                  // EBO allocation
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);             // EBO binding
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW); // Index data upload

    // Vertex attribute layout: location 0 = position, location 1 = normal
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);                                     // VAO unbind
    m.indexCount = (GLsizei)(sizeof(idx) / sizeof(unsigned int)); // Index count assignment
    return m;
}
