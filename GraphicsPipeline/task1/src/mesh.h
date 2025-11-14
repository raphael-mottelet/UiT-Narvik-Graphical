#pragma once
#include <glad/glad.h>

// simple mesh container for OpenGL objects
struct Mesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei indexCount = 0;
};

// builds a single triangle and returns the mesh handles
inline Mesh makeTriangle(){
    // Triangle in XY plane (Z=0)
    const float verts[] = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };
    const unsigned idx[] = {0,1,2};

    // create the mesh object we will fill
    Mesh m{};

    // create and bind a VAO to capture the buffer/attribute state
    glGenVertexArrays(1, &m.vao);
    glBindVertexArray(m.vao);

    // create a vertex buffer and upload the positions
    glGenBuffers(1, &m.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // create an index buffer and upload the triangle indices
    glGenBuffers(1, &m.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

    // describe vertex layout: location 0 = vec3 position, tightly packed
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    // done recording state into VAO
    glBindVertexArray(0);

    // one triangle = 3 indices
    m.indexCount = 3;
    return m;
}
